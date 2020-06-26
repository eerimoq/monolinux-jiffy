import threading
import logging
import queue
import serial


LOGGER = logging.getLogger(__name__)


class Serial(threading.Thread):

    def __init__(self, port_name):
        super().__init__()
        self.daemon = True
        self._serial = serial.Serial(port_name, 115200)
        self._response_queue = queue.Queue()
        self._response_lines = None
        self._line = b''

    def execute_command(self, command, timeout=2):
        self._response_lines = []
        self._serial.write(command.encode())
        self._serial.write(b'\n')

        try:
            return self._response_queue.get(timeout=timeout)
        except queue.Empty:
            raise TimeoutError(
                f"Command '{command}' timed out after {timeout} seconds.")

    def run(self):
        while True:
            line = self._serial.read_until()
            line = line.decode().strip()

            LOGGER.debug('%s', line)

            if self._response_lines is not None:
                self._response_lines.append(line)

                if line == 'OK':
                    response_lines = self._response_lines
                    self._response_lines = None
                    self._response_queue.put(response_lines)

            self._line = b''
