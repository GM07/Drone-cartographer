"""This module tests the ByteDecoder."""

from concurrent.futures import thread
from distutils.log import error
import threading
import time
from flask_socketio import SocketIO
import unittest
from unittest import mock

from pytest import console_main
from services.communication.comm_tasks import start_drone_status_task, start_logs_task, LOGS_QUEUE, LOGS_TASK_QUEUE_LOCK, __logs_task as log, __drone_status_task as status
import services.communication.comm_tasks as Task


class TestApplication(unittest.TestCase):

    def test_start_logs_task(self):
        mock_socket = SocketIO()
        return_value = mock.Mock()
        return_value.daemon = False
        mock_socket.start_background_task = mock.MagicMock(
            return_value=return_value)
        start_logs_task(mock_socket)
        mock_socket.start_background_task.assert_called_with(log, mock_socket)
        self.assertTrue(return_value.daemon)

    def test_start_drone_status_task(self):
        mock_socket = SocketIO()
        return_value = mock.Mock()
        return_value.daemon = False
        mock_socket.start_background_task = mock.MagicMock(
            return_value=return_value)
        start_drone_status_task(mock_socket)
        mock_socket.start_background_task.assert_called_once_with(
            status, mock_socket)
        self.assertTrue(return_value.daemon)

    def test_logs_tasks(self):

        mock_socket = SocketIO()
        Task.LOGS_QUEUE = mock.MagicMock()
        Task.LOGS_QUEUE.get = mock.MagicMock(return_value='test')
        mock_socket.emit = mock.MagicMock(return_value=0)
        th = threading.Thread(target=log, args=(mock_socket,))
        th.start()
        time.sleep(0.5)
        Task.logs_task_should_run = False

        th.join()
        mock_socket.emit.assert_called()

    def test_drone_task(self):
        mock_socket = SocketIO()
        Task.DRONE_STATUS_QUEUE = mock.MagicMock()
        Task.DRONE_STATUS_QUEUE.get = mock.MagicMock(return_value='test')
        mock_socket.emit = mock.MagicMock(return_value=0)
        th = threading.Thread(target=status, args=(mock_socket,))
        th.start()
        time.sleep(0.5)
        Task.drone_task_should_run = False

        th.join()
        mock_socket.emit.assert_called_with('update_drone_status',
                                            'test',
                                            namespace='/getDroneStatus',
                                            broadcast=True,
                                            include_self=False,
                                            skip_sid=True)
