"""This module contains the necessary functions used to
manage the local mission status

Example usage is :
from access_status as AccessStatus
AccessStatus.take_control(socket_io)

"""
from flask_socketio import SocketIO


NAMESPACE = '/limitedAccess'
controlling_client = None
is_mission_simulated = False

def set_mission_simulated(simulated: bool):
    global is_mission_simulated
    is_mission_simulated = simulated

def get_mission_simulated():
    return is_mission_simulated

def set_mission_type(socket_io: SocketIO, simulated: bool, exeception = None):
    global is_mission_simulated
    is_mission_simulated = simulated
    update_all_clients(socket_io, exeception)

def client_disconnected(socket_io: SocketIO, request):
    global controlling_client
    if controlling_client == request.sid:
        controlling_client = None
        update_all_clients(socket_io)
        return True

    return False

def take_control(socket_io: SocketIO, request):
    global controlling_client
    change = False
    if controlling_client is None:
        controlling_client = request.sid
        update_all_clients(socket_io)
        change = True
    else:
        update_specific_client(socket_io, request.sid)

    return change

def revoke_controlling_client(socket_io: SocketIO, request):
    global controlling_client
    change = False
    if controlling_client == request.sid:
        controlling_client = None
        update_all_clients(socket_io)
        change = True
    else:
        update_specific_client(socket_io, request.sid)

    return change


def update_all_clients(socket_io: SocketIO, exception = None):

    exception_list = []
    exception_list.append(controlling_client)
    exception_list.append(exception)

    socket_io.emit('update_status', get_mission_status(False),
    namespace=NAMESPACE, broadcast=True, include_self=False,
    skip_sid=exception_list)
    if controlling_client != exception and controlling_client is not None:
        update_specific_client(socket_io, controlling_client)

def update_specific_client(socket_io: SocketIO, request_session_id):
    status = get_mission_status(is_user_controlling(request_session_id))
    socket_io.emit('update_status', status, namespace=NAMESPACE,
    room=request_session_id)

def is_request_valid(request):
    return request is not None and request.sid == controlling_client

def get_mission_status(controlling):
    return {
        'isMissionSimulated': is_mission_simulated,
        'isUserControlling': controlling,
    }

def is_user_controlling(request_session_id):
    return controlling_client == request_session_id

def is_someone_controlling():
    return controlling_client is not None
