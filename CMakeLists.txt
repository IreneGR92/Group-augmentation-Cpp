cmake_minimum_required(VERSION 3.5)
project(group_augmentation C CXX)
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)


include_directories(${PROJECT_SOURCE_DIR}/include/)

if(UNIX AND NOT APPLE)
    message(STATUS "Linux x86_64")
    set(LIB_YAML ${PROJECT_SOURCE_DIR}/lib/libyaml-cpp.so)
else()
    message(STATUS "macOS")
    set(LIB_YAML ${PROJECT_SOURCE_DIR}/lib/libyaml-cpp.dylib)
endif()




set(SOURCE_FILES src/main.cpp src/Parameters.h src/Parameters.cpp src/Simulation.cpp src/Simulation.h src/model/Attribute.h src/model/Individual.h src/model/Individual.cpp src/model/Group.h src/model/Group.cpp src/model/Population.cpp src/model/Population.h src/stats/Statistics.h src/stats/Statistics.cpp src/stats/StatisticalFormulas.h src/stats/StatisticalFormulas.cpp src/model/container/IndividualVector.h src/model/container/IndividualVector.cpp)
add_executable(App ${SOURCE_FILES})
target_link_libraries(App PUBLIC ${LIB_YAML})


set(TEST_FILES src/test.cpp src/Parameters.h src/Parameters.cpp src/Simulation.cpp src/Simulation.h src/model/container/Container.h src/model/Attribute.h src/model/Individual.h src/model/Individual.cpp src/model/Group.h src/model/Group.cpp src/model/Population.cpp src/model/Population.h src/stats/Statistics.h src/stats/Statistics.cpp src/stats/StatisticalFormulas.h src/stats/StatisticalFormulas.cpp src/model/container/IndividualVector.h src/model/container/IndividualVector.cpp)
add_executable(Test ${TEST_FILES})
target_link_libraries(Test PUBLIC ${LIB_YAML})




