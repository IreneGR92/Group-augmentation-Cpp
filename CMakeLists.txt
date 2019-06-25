cmake_minimum_required(VERSION 3.0)
project("Group Augmentation")
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_INSTALL_NAME_DIR build)

set(SOURCE_FILES src/main.cpp src/Parameters.h src/Parameters.cpp)

add_executable(App ${SOURCE_FILES})