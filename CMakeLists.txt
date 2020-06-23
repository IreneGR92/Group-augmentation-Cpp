cmake_minimum_required(VERSION 3.5)
project(group_augmentation C CXX)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
#

#include_directories(${CMAKE_SOURCE_DIR}/src)
#link_directories(${CMAKE_SOURCE_DIR}/lib)
##For the shared library:
#
#

#
#
#target_link_libraries(App PUBLIC)


find_library(yaml-cpp SHARED IMPORTED)

#set_target_properties(yaml-cpp PROPERTIES IMPORTED_LOCATION ../yaml-cpp/libyaml-cpp.so)
#pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

include_directories(${PROJECT_SOURCE_DIR}/include/yaml-cpp)

set(SOURCE_FILES src/main.cpp src/headers/Parameters.h src/implementation/Parameters.cpp)
add_executable(App ${SOURCE_FILES})
target_link_libraries(App yaml-cpp)



