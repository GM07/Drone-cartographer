
# enable app support
APP=1
APP_STACKSIZE=300

VPATH += src/
PROJ_OBJ += commands_handler.o communication_manager.o firmware_controller.o navigation_system.o sensor_manager.o

CRAZYFLIE_BASE=../crazyflie-firmware

CXXFLAGS+=-I../crazyflie-firmware/src/modules/
CXXFLAGS+=-I../software-bridge/include
CLOAD_CMDS = -w radio://0/80/2M/E7E7E7E761


include $(CRAZYFLIE_BASE)/Makefile
