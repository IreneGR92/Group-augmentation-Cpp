cmake_minimum_required(VERSION 3.5)
project(group_augmentation C CXX)
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_FILES src/main.cpp src/Parameters.h src/Parameters.cpp src/model/Individual.cpp src/model/Individual.h src/model/FishType.h src/model/Group.cpp src/model/Group.h src/Simulation.cpp src/Simulation.cpp src/Simulation.h src/stats/Statistics.cpp src/stats/Statistics.h src/stats/StatisticalSum.cpp src/stats/StatisticalSum.h src/model/Attribute.h src/model/IndividualVector.cpp src/model/IndividualVector.h)
#set(YAML_DIR ../yaml-cpp)


#For the shared library:
set(YAML_LIBRARY libyaml-cpp.so)
link_directories(../yaml-cpp)


include_directories(../yaml-cpp/include)


add_executable(App ${SOURCE_FILES})



target_link_libraries(App PUBLIC ~/git/yaml-cpp/libyaml-cpp.so)





