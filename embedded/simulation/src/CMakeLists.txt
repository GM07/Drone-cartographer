FILE(GLOB PROJECT_SRC *.cc)
add_library(simulation_firmware SHARED ${PROJECT_SRC})
target_compile_features(simulation_firmware PRIVATE cxx_std_17)