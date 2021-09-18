CMAKE_MINIMUM_REQUIRED(VERSION 3.18) #cmake版本要求


message("Building cmdline") #打印消息


set(SETTINGENV done)

# SET(CMAKE_C_COMPILER "gcc")
# SET(CMAKE_CXX_COMPILER "g++")


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
#set(CMAKE_BUILD_TYPE release)



# 设置debug或者release的编译参数
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -W -Wall -Wextra -Wreturn-type -g -ggdb -fPIC -std=c++2a")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Os -W -Wall -Wextra -Wreturn-type -fPIC -std=c++2a")

set(ABSL_MODULE_PATH "../abseil-cpp")

include_directories(${ABSL_MODULE_PATH})
link_directories(${ABSL_MODULE_PATH})
add_subdirectory(${ABSL_MODULE_PATH} build/${ABSL_MODULE_PATH}.out) # add a submodule, and compile it to the "build/lib"



#分别指定编译include和链接link的搜索目录
# include_directories(../include)

aux_source_directory(. DIR_LIB_SRCS)
MESSAGE("building cmdline ... ")
add_library(cmdline STATIC ${DIR_LIB_SRCS} )
target_link_libraries(cmdline absl::strings)
target_include_directories(cmdline PUBLIC ${PROJECT_SOURCE_DIR}/src)