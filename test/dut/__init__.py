import subprocess
import os
import logging
from hashlib import sha256
from .bunga import ClientThread as BungaClientThread
from .serial import Serial
import bunga
from humanfriendly import format_size


LOGGER = logging.getLogger(__name__)


COREDUMP_FMT = '''\
CORE
------------------------------------------------------------------
{core}
LOG
------------------------------------------------------------------
{log}
'''


class Coredump:

    def __init__(self, corefile, logfile):
        with open(corefile, 'rb') as fin:
            self.core = fin.read()

        with open(logfile, 'rb') as fin:
            self.log = fin.read()

        command = [
            'gdb-multiarch', 'app/build/app.debug', corefile,
            '--batch',
            '-ex', 'bt 25'
        ]
        core = subprocess.run(command, capture_output=True)
        stderr = core.stderr.decode()

        if 'is truncated: expected core file size' in stderr:
            for line in stderr.splitlines():
                LOGGER.info('GDB stderr: %s', line.strip())

            raise Exception('Corrupt coredump. See log for details.')

        self.report = COREDUMP_FMT.format(core=core.stdout.decode(),
                                          log=self.log.decode())

    def __eq__(self, other):
        if self.core != other.core:
            return False

        if self.log != other.log:
            return False

        return True


class Dut:

    def __init__(self, serial_port):
        self.serial = Serial(serial_port)
        self.serial.start()
        self.bunga_client = None
        self.ip_address = None

    def start_bunga_client(self):
        self.bunga_client = BungaClientThread(f'tcp://{self.ip_address}:28000')
        self.bunga_client.start()
        self.bunga_client.wait_for_connection()

    def execute_command(self, command):
        return self.bunga_client.execute_command(command)

    def get_file(self, path):
        local_path = os.path.basename(path)
        self.bunga_client.wait_for_connection()
        LOGGER.debug("Getting file '%s' to '%s'.", path, local_path)
        self.bunga_client.get_file(path, local_path)

        with open(local_path, 'rb') as fin:
            data = fin.read()

        LOGGER.debug("'%s' is %s with sha256 %s.",
                     local_path,
                     format_size(os.stat(local_path).st_size),
                     sha256(data).hexdigest())

    def list_coredumps(self):
        try:
            output = self.execute_command('ls disk/coredumps')
        except bunga.client.ExecuteCommandError:
            output = ''

        return [int(slot) for slot in output.split()]

    def remove_coredumps(self):
        for slot in self.list_coredumps():
            self.rm(f'/disk/coredumps/{slot}/core')
            self.rm(f'/disk/coredumps/{slot}/log.txt')
            self.rm(f'/disk/coredumps/{slot}/info.txt')
            self.rm(f'/disk/coredumps/{slot}')

        self.sync()

    def get_coredumps(self):
        coredumps = []

        for slot in self.list_coredumps():
            self.get_file(f'/disk/coredumps/{slot}/core')
            self.get_file(f'/disk/coredumps/{slot}/log')
            coredumps.append(Coredump('core', 'log'))

        return coredumps

    def rm(self, path):
        try:
            self.execute_command(f'rm {path}')
        except bunga.client.ExecuteCommandError as e:
            if e.error != 'No such file or directory':
                raise

    def sync(self):
        self.execute_command('sync')

    def wait_for_connection(self, timeout=30):
        self.bunga_client.wait_for_connection(timeout)
