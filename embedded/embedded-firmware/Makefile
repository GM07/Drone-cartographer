
# enable app support
APP=1
APP_STACKSIZE=300

VPATH += src/
VPATH += ../shared-firmware/
PROJ_OBJ += commands_handler.o communication_manager.o firmware_controller.o navigation_system.o timer.o app_main.o vector3d.o

CRAZYFLIE_BASE=../crazyflie-firmware

CXXFLAGS+=-I../crazyflie-firmware/src/modules/
CXXFLAGS+=-I../shared-firmware/include/
CXXFLAGS+=-Iinclude/


include $(CRAZYFLIE_BASE)/Makefile
