cmake_minimum_required(VERSION 3.8.2)
project(my_app_project)

# Pick the C++ standard to compile with.
# Abseil currently supports C++11, C++14, and C++17.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(abseil-cpp)

aux_source_directory(./src DIR_LIB_SRCS)
MESSAGE("building cmdlines ... ")
add_library(cmdlines STATIC ${DIR_LIB_SRCS} )
target_link_libraries(cmdlines absl::base absl::synchronization absl::strings glog)
target_include_directories(cmdlines PUBLIC ${PROJECT_SOURCE_DIR}/src)

include_directories(./src)

aux_source_directory(./example SRC_LIST)

add_executable(my_app_project ${SRC_LIST})
target_link_libraries(my_app_project 
                        cmdlines)