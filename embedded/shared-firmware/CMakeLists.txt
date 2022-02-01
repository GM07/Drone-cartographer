FILE(GLOB PROJECT_SRC *.cc)
add_library(shared_firmware SHARED ${PROJECT_SRC})