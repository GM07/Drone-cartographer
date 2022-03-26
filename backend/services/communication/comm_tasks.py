from asyncio.subprocess import PIPE, STDOUT
import gevent.queue
import threading
from flask_socketio import SocketIO

LOGS_TASK_QUEUE_LOCK = threading.Lock()
DRONE_STATUS_QUEUE_LOCK = threading.Lock()
LOGS_QUEUE = gevent.queue.Queue(-1)
logs_task_should_run = True
DRONE_STATUS_QUEUE = gevent.queue.Queue(-1)


def start_logs_task(SOCKETIO: SocketIO):

    if LOGS_TASK_QUEUE_LOCK.acquire(blocking=False):
        thread = SOCKETIO.start_background_task(__logs_task, SOCKETIO)
        thread.daemon = True


def start_drone_status_task(SOCKETIO: SocketIO):

    if DRONE_STATUS_QUEUE_LOCK.acquire(blocking=False):
        thread = SOCKETIO.start_background_task(__drone_status_task, SOCKETIO)
        thread.daemon = True


def __logs_task(SOCKETIO: SocketIO):
    while logs_task_should_run:
        logs = LOGS_QUEUE.get()
        SOCKETIO.emit('get_logs',
                      logs,
                      namespace='/getLogs',
                      broadcast=True,
                      include_self=False,
                      skip_sid=True)


def __drone_status_task(SOCKETIO: SocketIO):
    while True:
        status = DRONE_STATUS_QUEUE.get()
        SOCKETIO.emit('update_drone_status',
                      status,
                      namespace='/getDroneStatus',
                      broadcast=True,
                      include_self=False,
                      skip_sid=True)