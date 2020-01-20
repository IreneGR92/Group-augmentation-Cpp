cmake_minimum_required(VERSION 3.5)
project(group_augmentation C CXX)
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_FILES src/main.cpp src/Parameters.h src/Parameters.cpp src/model/Individual.cpp src/model/Individual.h src/model/Classes.h src/model/Group.cpp src/model/Group.h)
#set(YAML_DIR ../yaml-cpp)


#For the shared library:
set(YAML_LIBRARY libyaml-cpp.so)
link_directories(../yaml-cpp)


include_directories(../yaml-cpp/include)


add_executable(App ${SOURCE_FILES})



target_link_libraries(App PUBLIC ~/git/yaml-cpp/libyaml-cpp.so.0.6.2)





