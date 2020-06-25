cmake_minimum_required(VERSION 3.5)
project(group_augmentation C CXX)
set(CMAKE_CXX_STANDARD 14)
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

set(SOURCE_FILES src/main.cpp src/Parameters.h src/Parameters.cpp src/model/Group.h src/model/Group.cpp src/model/Individual.h src/model/Individual.cpp src/model/IndividualVector.h src/model/IndividualVector.cpp src/model/Attribute.h src/model/DataModel.cpp src/model/DataModel.h)
add_executable(App ${SOURCE_FILES})
target_link_libraries(App yaml-cpp)



