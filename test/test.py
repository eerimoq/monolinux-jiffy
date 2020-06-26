#!/usr/bin/env python3

import subprocess
import time
import re
import logging
from io import BytesIO
import systest
import bunga
from dut import Dut
from humanfriendly import format_timespan
from humanfriendly import format_size


LOGGER = logging.getLogger(__name__)


class TestCase(systest.TestCase):

    def __init__(self, dut):
        super().__init__()
        self.dut = dut


class StartUpTest(TestCase):
    """Preparations for other tests.

    """

    def get_ip_address(self):
        # DHCP is slow sometimes.
        end_time = time.time() + 30

        while time.time() < end_time:
            try:
                lines = self.dut.serial.execute_command('ifconfig eth0')
            except TimeoutError:
                pass
            else:
                output = ' '.join(lines)
                mo = re.search(r'IPAddress:  (\d+\.\d+\.\d+\.\d+)', output)

                if mo:
                    self.dut.ip_address = mo.group(1)
                    break

                time.sleep(0.5)
        else:
            raise Exception('Timeout waiting for IP address.')

    def run(self):
        self.get_ip_address()
        self.dut.start_bunga_client()
        self.dut.remove_coredumps()


class CoredumpTest(TestCase):
    """Intentionally crash the system.

    """

    def suicide_abort(self):
        with self.assert_raises(bunga.client.ExecuteCommandError) as cm:
            self.dut.execute_command('suicide abort')

        self.assert_equal(str(cm.exception.error), 'Connection lost.')

    def run(self):
        self.dut.execute_command('log print emergency CoredumpTest')
        self.suicide_abort()
        self.dut.wait_for_connection()
        coredumps = self.dut.get_coredumps()

        self.assert_equal(len(coredumps), 1)
        coredump = coredumps[0]
        self.assert_in(
            'Program terminated with signal SIGILL, Illegal instruction.',
            coredump.report)
        self.assert_in(' in command_suicide (', coredump.report)
        self.assert_in(' in ml_shell_execute_command (', coredump.report)
        self.assert_in('EMERGENCY default CoredumpTest', coredump.report)

        for line in coredump.report.splitlines():
            LOGGER.debug('coredump: %s', line.strip())

        self.dut.remove_coredumps()


class ReadTemperatureTest(TestCase):
    """Read the temperature.

    """

    @systest.xfail('The device is not always found.')
    def run(self):
        output = self.dut.execute_command('ds18b20 read 280000055d329533')
        self.assert_is_not_none(re.match(r'Temperature: \d+\.\d+ C', output))



class NtpSyncTest(TestCase):
    """Set the device time using NTP.

    """

    def run(self):
        self.dut.execute_command('date 10000000')
        output = self.dut.execute_command('date')
        self.assert_in('1970', output)
        self.dut.execute_command('ntp_sync')
        output = self.dut.execute_command('date')
        self.assert_not_in('1970', output)


class HttpGetTest(TestCase):
    """Perform an HTTP GET.

    """

    def run(self):
        self.dut.execute_command('http_get http://example.com')


class DiskTest(TestCase):
    """Read and write files on disk.

    """

    def run(self):
        self.dut.rm('/disk/test.txt')
        self.dut.execute_command('print test /disk/test.txt')
        output = self.dut.execute_command('cat /disk/test.txt')
        self.assert_equal(output, 'test\n')


class BungaPutAndGetSpeedTest(TestCase):
    """Measure get and put file speeds.

    """

    def run(self):
        file_size = 1_000_000
        remote_path = '/disk/bunga-put-and-get-speed-test.bin'

        fp = BytesIO()
        fp.write(file_size // 10 * b'1234567890')
        fp.seek(0)

        # Put.
        start_time = time.time()
        self.dut.bunga_client.put_file(fp, file_size, remote_path)
        elapsed_time = time.time() - start_time

        LOGGER.info('Put %s in %s (%.02f MB/s).',
                    format_size(file_size),
                    format_timespan(elapsed_time),
                    file_size / 1_000_000 / elapsed_time)

        # Get.
        start_time = time.time()
        self.dut.bunga_client.get_file(remote_path,
                                       'bunga-put-and-get-speed-test.bin')
        elapsed_time = time.time() - start_time

        LOGGER.info('Got %s in %s (%.02f MB/s).',
                    format_size(file_size),
                    format_timespan(elapsed_time),
                    file_size / 1_000_000 / elapsed_time)

        with open('bunga-put-and-get-speed-test.bin', 'rb') as fin:
            actual = fin.read()

        self.assert_equal(len(actual), file_size)

        expected = fp.getvalue()

        for offset in range(file_size):
            try:
                self.assert_equal(actual[offset], expected[offset])
            except systest.TestCaseFailedError:
                LOGGER.error('Offset: %d, Actual: %s, Expected %s',
                             offset,
                             actual[offset - 10:offset + 10],
                             expected[offset - 10:offset + 10])
                raise


class CoreDumpCheckTest(TestCase):
    """Check if there is a coredump. Only the latest coredump is saved.

    """

    def run(self):
        self.dut.wait_for_connection()
        coredumps = self.dut.get_coredumps()

        for coredump in coredumps:
            for line in coredump.report.splitlines():
                LOGGER.debug('coredump: %s', line.strip())

        self.assert_equal(len(coredumps), 0)


class PrintStatsTest(TestCase):
    """Print some stats.

    """

    def run(self):
        self.dut.execute_command('uptime')
        self.dut.execute_command('ps')
        self.dut.execute_command('cat proc/meminfo')
        self.dut.execute_command('cat proc/interrupts')
        self.dut.execute_command('cat proc/diskstats')


def main():
    sequencer = systest.setup("Monolinux Jiffy")

    dut = Dut('/dev/ttyUSB0')

    sequencer.run(
        StartUpTest(dut),
        [
            # CoredumpTest(dut),
            DiskTest(dut),
            NtpSyncTest(dut),
            HttpGetTest(dut),
            ReadTemperatureTest(dut),
            BungaPutAndGetSpeedTest(dut),
            CoreDumpCheckTest(dut),
            PrintStatsTest(dut)
        ]
    )

    sequencer.report_and_exit()


main()
