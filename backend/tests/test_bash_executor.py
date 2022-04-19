from multiprocessing import Process
import threading
from time import sleep
import unittest

from unittest.mock import MagicMock, patch
from weakref import ref
from services.communication.bash_executor import BashExecutor


class TestBashExecutor(unittest.TestCase):

    def test_stop(self):
        socket_mock = MagicMock()
        process_mock = MagicMock()
        on_end_mock = MagicMock()

        bash = BashExecutor('test', socket_mock, '/test')
        bash.process = process_mock
        bash.on_end = on_end_mock
        bash.stop()
        socket_mock.emit.assert_called_with('stop',
                                            namespace=bash.namespace,
                                            broadcast=True,
                                            include_self=False,
                                            skip_sid=True)
        process_mock.kill.assert_called()
        on_end_mock.assert_called()

    def test_change_command(self):
        socket_mock = MagicMock()
        bash = BashExecutor('first', socket_mock, '/test')
        bash.change_command('second')
        assert bash.bash_command == 'second'

    @patch('subprocess.Popen', MagicMock())
    def test_start(self):
        socket_mock = MagicMock()
        bash = BashExecutor('test', socket_mock, '/test')
        bash.start()
        socket_mock.emit.assert_called_with('start',
                                            namespace=bash.namespace,
                                            broadcast=True,
                                            include_self=False,
                                            skip_sid=True)

    @patch('services.communication.bash_executor.BashExecutor.stop',
           MagicMock())
    def test_transmit_stdout(self):
        socket_mock = MagicMock()
        process_mock = MagicMock()
        methods = {
            'poll.return_value': 'test',
            'stdout.read.return_value': "test".encode('UTF-8')
        }
        process_mock.configure_mock(**methods)
        bash = BashExecutor('test', socket_mock, '/test')
        bash.process = process_mock
        bash.nb_output_finished = 1
        bash._BashExecutor__transmit_stdout()
        socket_mock.emit.assert_called_with('stdout',
                                            'test',
                                            namespace=bash.namespace,
                                            broadcast=True,
                                            include_self=False,
                                            skip_sid=True)
        bash.stop.assert_called()

    @patch('services.communication.bash_executor.BashExecutor.stop',
           MagicMock())
    def test_transmit_stderr(self):
        socket_mock = MagicMock()
        process_mock = MagicMock()
        methods = {
            'poll.return_value': 'test',
            'stderr.read.return_value': "test".encode('UTF-8')
        }
        process_mock.configure_mock(**methods)
        bash = BashExecutor('test', socket_mock, '/test')
        bash.process = process_mock
        bash.nb_output_finished = 1
        bash._BashExecutor__transmit_stderr()
        socket_mock.emit.assert_called_with('stderr',
                                            'test',
                                            namespace=bash.namespace,
                                            broadcast=True,
                                            include_self=False,
                                            skip_sid=True)
        bash.stop.assert_called()
