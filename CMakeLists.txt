cmake_minimum_required(VERSION 3.5)
project("group_augmentation")
set(CMAKE_CXX_STANDARD 14)



set(SOURCE_FILES src/main.cpp src/Parameters.h src/Parameters.cpp)

find_package(yaml-cpp REQUIRED)
add_executable(App ${SOURCE_FILES})

set(CMAKE_BUILD_TYPE Release)

target_link_libraries(App PUBLIC yaml-cpp)
#add_executable(yaml_cpp_test yaml_cpp_test.cpp)
#target_link_libraries(yaml_cpp_test yaml-cpp)





