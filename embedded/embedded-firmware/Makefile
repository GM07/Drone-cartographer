
# enable app support
APP=1
APP_STACKSIZE=300

VPATH += src/
VPATH += ../shared-firmware/src/
PROJ_OBJ += commands_handler.o communication_manager.o firmware_controller.o navigation_system.o timer.o app_main.o vector3d.o

CRAZYFLIE_BASE=../crazyflie-firmware

<<<<<<< HEAD
CXXFLAGS += -I../crazyflie-firmware/src/modules/
CXXFLAGS += -I../shared-firmware/include/
CXXFLAGS += '-Wno-write-strings'
=======
CXXFLAGS+=-I../crazyflie-firmware/src/modules/
CXXFLAGS+=-I../shared-firmware/include/
CXXFLAGS+=-Iinclude/

CLOAD_CMDS = -w $(radio)
>>>>>>> 41dceeb79f1823b5213a9da31229d4f6e7029823

include $(CRAZYFLIE_BASE)/Makefile
