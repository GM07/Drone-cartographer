from threading import Lock
from flask import Flask, render_template, session, request, \
    copy_current_request_context
from flask_socketio import SocketIO, emit, join_room, leave_room, \
    close_room, rooms, disconnect
import numpy as np

# Set this variable to "threading", "eventlet" or "gevent" to test the
# different async modes, or leave it set to None for the application to choose
# the best option based on installed packages.
async_mode = 'gevent'

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app, async_mode=async_mode, cors_allowed_origins='*')
thread = None
thread_lock = Lock()


def background_thread():
    """Example of how to send server generated events to clients."""
    count = 0
    data = []
    while True:
        socketio.sleep(0.5)
        count += 1
        data = [np.random.randint(1,10), 
        np.random.randint(1,10), 
        np.random.randint(1,10), 
        np.random.randint(1,10)]
        socketio.emit('getMapData', data, namespace='/getMapData')


@socketio.event(namespace='/getMapData')
def connect():
    print("connect")
    global thread
    with thread_lock:
        if thread is None:
            thread = socketio.start_background_task(background_thread)


@socketio.on('disconnect')
def test_disconnect():
    print('Client disconnected', request.sid)


if __name__ == '__main__':
    socketio.run(app, port=5000)
