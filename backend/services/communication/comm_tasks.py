from asyncio.subprocess import PIPE, STDOUT
import gevent.queue
import threading
import shlex, subprocess
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


def start_recompile_task(SOCKETIO: SocketIO):
    simulationThread = SOCKETIO.start_background_task(
        __recompile_simulation_task, SOCKETIO)
    embeddedThread = SOCKETIO.start_background_task(__recompile_embedded_task,
                                                    SOCKETIO)
    simulationThread.daemon = True
    embeddedThread.daemon = True


def __recompile_simulation_task(SOCKETIO: SocketIO):
    recompileSimulationCommands = [
        "cd /workspaces/INF3995-106/embedded/simulation",
        "if test -d build; then rm -rf build; fi", "mkdir build", "cd build",
        "cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../../",
        "ninja"
    ]
    recompileSimulation = "docker exec embedded sh -c '" + " && ".join(
        recompileSimulationCommands) + "'"

    recompile_simulation_process = subprocess.Popen(
        shlex.split(recompileSimulation), stdout=PIPE, stderr=STDOUT)

    while (recompile_simulation_process.poll() == None):
        stdout = recompile_simulation_process.stdout.readline().decode()
        SOCKETIO.emit('recompile_simulation',
                      stdout,
                      namespace='/recompileSimulation',
                      broadcast=True,
                      include_self=False,
                      skip_sid=True)


def __recompile_embedded_task(SOCKETIO: SocketIO):
    recompileEmbeddedCommands = [
        "cd /workspaces/INF3995-106/embedded/embedded-firmware", "make clean",
        "make -j`nproc`"
    ]

    recompileEmbedded = "docker exec embedded sh -c '" + " && ".join(
        recompileEmbeddedCommands) + "'"

    recompile_embedded_process = subprocess.Popen(
        shlex.split(recompileEmbedded), stdout=PIPE, stderr=STDOUT)

    while (recompile_embedded_process.poll() == None):
        stdout = recompile_embedded_process.stdout.readline().decode()
        SOCKETIO.emit('recompile_embedded',
                      stdout,
                      namespace='/recompileEmbedded',
                      broadcast=True,
                      include_self=False,
                      skip_sid=True)
