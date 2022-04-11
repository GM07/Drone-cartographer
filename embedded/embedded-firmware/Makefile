
# enable app support
APP=1
APP_STACKSIZE=1000

VPATH += src/
VPATH += ../shared-firmware/src/
PROJ_OBJ += commands_handler.o \
	communication_manager.o \
	firmware_controller.o \
	navigation_system.o \
	time.o \
	app_main.o \
	vector3d.o \
	firmware_sensors.o \
	sensors_data_updater.o \
	p2p_task.o \
	math.o \
	color_gradient.o

CRAZYFLIE_BASE=../crazyflie-firmware

CXXFLAGS +=-I../crazyflie-firmware/src/modules/
CXXFLAGS +=-I../shared-firmware/include/
CXXFLAGS += '-Wno-write-strings'
CXXFLAGS +=-Iinclude/

CLOAD_CMDS = -w $(radio)

include $(CRAZYFLIE_BASE)/Makefile
