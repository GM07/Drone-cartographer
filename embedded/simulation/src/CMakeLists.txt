FILE(GLOB PROJECT_SRC *.cc)
add_library(simulation_firmware SHARED ${PROJECT_SRC})