

from constants import COMMANDS


class AbstractComm:
    DELAY_RECEIVER_MS = 1000

    def send_command(self, command: COMMANDS) -> None:
        pass
