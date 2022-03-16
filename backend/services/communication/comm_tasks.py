import gevent.queue
import threading
from flask_socketio import SocketIO

LOGS_TASK_QUEUE_LOCK = threading.Lock()
LOGS_QUEUE = gevent.queue.Queue(-1)


def start_logs_task(SOCKETIO: SocketIO):

    if LOGS_TASK_QUEUE_LOCK.acquire(blocking=False):
        thread = SOCKETIO.start_background_task(__logs_task, SOCKETIO)
        thread.daemon = True


def __logs_task(SOCKETIO: SocketIO):
    while True:
        logs = LOGS_QUEUE.get()
        print("in", logs)
        SOCKETIO.emit('get_logs',
                      logs,
                      namespace='/getLogs',
                      broadcast=True,
                      include_self=False,
                      skip_sid=True)
