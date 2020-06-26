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


#set_target_properties(yaml-cpp PROPERTIES IMPORTED_LOCATION ../yaml-cpp/libyaml-cpp.so)
#pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)

include_directories(${PROJECT_SOURCE_DIR}/include/)
set(LIB_YAML ${PROJECT_SOURCE_DIR}/../yaml-cpp/libyaml-cpp.so)


set(SOURCE_FILES src/main.cpp src/Parameters.h src/Parameters.cpp src/model/Attribute.h src/model/container/Container.h src/model/Individual.h src/model/Individual.cpp)
add_executable(App ${SOURCE_FILES})
target_link_libraries(App PUBLIC ${LIB_YAML})


set(TEST_FILES src/test.cpp src/Parameters.h src/Parameters.cpp src/model/container/Container.h src/model/Attribute.h src/model/container/Container.h src/model/Individual.h src/model/Individual.cpp)
add_executable(Test ${TEST_FILES})
target_link_libraries(Test PUBLIC ${LIB_YAML})




