FILE(GLOB PROJECT_SRC *.cc)
add_library(shared_firmware SHARED ${PROJECT_SRC})
target_compile_features(shared_firmware PRIVATE cxx_std_17)