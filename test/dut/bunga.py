import logging
import bunga


LOGGER = logging.getLogger(__name__)


class Client(bunga.Client):

    async def on_disconnected(self):
        if self._is_connected:
            LOGGER.info('Disconnected')

        await super().on_disconnected()

    async def on_connect_rsp(self, message):
        await super().on_connect_rsp(message)
        LOGGER.info('Connected')

    async def on_log_entry_ind(self, message):
        LOGGER.debug('%s', ''.join(message.text))


class ClientThread(bunga.ClientThread):

    def __init__(self, uri):
        super().__init__(uri, Client)

    def execute_command(self, command):
        LOGGER.debug('%s', command)

        output = super().execute_command(command).decode()

        for line in output.splitlines():
            LOGGER.debug('%s', line)

        return output
