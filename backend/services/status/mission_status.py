is_mission_simulated = False
is_mission_started = False

# TODO : Discuter si c'est vraiment necessaire d'avoir ces fonctions
#        puisque les variables sont globales

def set_mission_started(started):
    global is_mission_started
    is_mission_started = started

def set_mission_simulated(simulated):
    global is_mission_simulated
    is_mission_simulated = simulated

def get_mission_started():
    return is_mission_started

def get_mission_simulated():
    return is_mission_simulated

def get_mission_status():
    return {
        'isMissionStarted': is_mission_started,
        'isMissionSimulated': is_mission_simulated,
    }
