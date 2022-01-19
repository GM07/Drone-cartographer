
# enable app support
APP=1
APP_STACKSIZE=300

VPATH += src/
PROJ_OBJ += commandsHandler.o communicationManager.o firmwareController.o navigationSystem.o sensorManager.o

CRAZYFLIE_BASE=../crazyflie-firmware

CXXFLAGS+=-I../software-bridge/include
CXXFLAGS+=-I../crazyflie-firmware/src/modules/interface/

include $(CRAZYFLIE_BASE)/Makefile
