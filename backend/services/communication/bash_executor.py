import subprocess
from flask_socketio import SocketIO
import shlex


class BashExecutor:

    def __init__(self, bashCommand: str, SOCKETIO: SocketIO, namespace: str):
        self.bashCommand: str = bashCommand
        self.SOCKETIO: SocketIO = SOCKETIO
        self.namespace: str = namespace
        self.process = None
        self.transmitStdoutTask = None
        self.transmitStderrTask = None
        self.nbOutputFinished = 0

    def stop(self):
        self.SOCKETIO.emit('stop',
                           namespace=self.namespace,
                           broadcast=True,
                           include_self=False,
                           skip_sid=True)

        if (self.process != None):
            self.process.kill()

    def changeCommand(self, bashCommand: str):
        self.bashCommand = bashCommand

    def start(self):
        # Make sure we are not running the same process twice
        self.stop()
        self.nbOutputFinished = 0

        self.process = subprocess.Popen(shlex.split(self.bashCommand),
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE)

        self.SOCKETIO.emit('start',
                           namespace=self.namespace,
                           broadcast=True,
                           include_self=False,
                           skip_sid=True)

        self.transmitStdoutTask = self.SOCKETIO.start_background_task(
            self.__transmit_stdout)
        self.transmitStdoutTask.daemon = True

        self.transmitStderrTask = self.SOCKETIO.start_background_task(
            self.__transmit_stderr)
        self.transmitStderrTask.daemon = True

    def __transmit_stdout(self):
        while (self.process.poll() is None):
            stdout = self.process.stdout.read(1).decode()
            if stdout != "":
                self.SOCKETIO.emit('stdout',
                                   stdout,
                                   namespace=self.namespace,
                                   broadcast=True,
                                   include_self=False,
                                   skip_sid=True)

        # Sends whatever is left in the pipe
        stdout = self.process.stdout.read().decode()
        if stdout != "":
            self.SOCKETIO.emit('stdout',
                               stdout,
                               namespace=self.namespace,
                               broadcast=True,
                               include_self=False,
                               skip_sid=True)
        self.nbOutputFinished += 1
        if (self.nbOutputFinished == 2):
            self.stop()

    def __transmit_stderr(self):
        while (self.process.poll() is None):
            stderr = self.process.stderr.read(1).decode()
            if stderr != "":
                self.SOCKETIO.emit('stderr',
                                   stderr,
                                   namespace=self.namespace,
                                   broadcast=True,
                                   include_self=False,
                                   skip_sid=True)

        # Sends whatever is left in the pipe
        stderr = self.process.stderr.read().decode()
        if stderr != "":
            self.SOCKETIO.emit('stderr',
                               stderr,
                               namespace=self.namespace,
                               broadcast=True,
                               include_self=False,
                               skip_sid=True)
        self.nbOutputFinished += 1
        if self.nbOutputFinished == 2:
            self.stop()

    def __del__(self):
        self.stop()