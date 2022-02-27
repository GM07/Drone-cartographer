
# enable app support
APP=1
APP_STACKSIZE=300

VPATH += src/
VPATH += ../shared-firmware/src/
PROJ_OBJ += commands_handler.o communication_manager.o firmware_controller.o navigation_system.o timer.o app_main.o vector3d.o firmware_sensor.o sensor_data_updater.o

CRAZYFLIE_BASE=../crazyflie-firmware

CXXFLAGS +=-I../crazyflie-firmware/src/modules/
CXXFLAGS +=-I../shared-firmware/include/
CXXFLAGS += '-Wno-write-strings'
CXXFLAGS +=-Iinclude/

CLOAD_CMDS = -w $(radio)

include $(CRAZYFLIE_BASE)/Makefile
