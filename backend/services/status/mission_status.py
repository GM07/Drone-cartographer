"""This module contains the necessary functions used to
manage the global mission status

Example usage is :
from mission_status as MissionStatus
MissionStatus.launch_mission(socket_io)

"""
from flask_socketio import SocketIO
import services.status.access_status as AccessStatus

NAMESPACE = '/getMissionStatus'
is_mission_started = False

# TODO : Discuter si c'est vraiment necessaire d'avoir ces fonctions
#        puisque les variables sont globales


def set_mission_started(started: bool):
    global is_mission_started
    is_mission_started = started


def get_mission_started():
    return is_mission_started


def client_connected(socket_io: SocketIO, request):
    update_specific_client(socket_io, request.sid)


def launch_mission(socket_io: SocketIO):
    global is_mission_started
    is_mission_started = True
    update_all_clients(socket_io)


def terminate_mission(socket_io: SocketIO):
    global is_mission_started
    is_mission_started = False
    update_all_clients(socket_io)


def update_all_clients(socket_io: SocketIO, exception=None):
    exception_list = []
    if exception is not None:
        exception_list.append(exception)

    socket_io.emit('update_status',
                   get_mission_status(),
                   namespace=NAMESPACE,
                   broadcast=True,
                   include_self=False,
                   skip_sid=exception_list)


def update_specific_client(socket_io: SocketIO, request_session_id):
    socket_io.emit('update_status',
                   get_mission_status(),
                   namespace=NAMESPACE,
                   room=request_session_id)


def get_mission_status():
    return {
        'isMissionStarted': is_mission_started,
        'isSomeoneControlling': AccessStatus.is_someone_controlling(),
    }
