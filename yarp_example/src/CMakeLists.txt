
cmake_minimum_required(VERSION 2.6)

set(PROJECTNAME yarp_example)
project(${PROJECTNAME})

find_package(YARP)

add_executable(yarp_example yarp_example.cpp)
target_link_libraries(yarp_example ${YARP_LIBRARIES})
