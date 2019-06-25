cmake_minimum_required(VERSION 3.5)
project("group_augmentation")
set(CMAKE_CXX_STANDARD 14)


find_library(YAML yaml-cpp REQUIRED)

set(SOURCE_FILES src/main.cpp src/Parameters.h src/Parameters.cpp)
add_executable(App ${SOURCE_FILES})

set(CMAKE_BUILD_TYPE Release)

target_link_libraries(App PUBLIC ${YAML})
#add_executable(yaml_cpp_test yaml_cpp_test.cpp)
#target_link_libraries(yaml_cpp_test yaml-cpp)





