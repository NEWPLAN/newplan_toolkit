CMAKE_MINIMUM_REQUIRED(VERSION 3.0) #cmake版本要求
project(ipc_benchmark)

message("Building make for tcp_benchmark") #打印消息
message("add -DDYNAMIC_E=1 when build dynamic class E, the default is static")
set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_BUILD_TYPE "Release")

# set(SUB_MODULE_PATH "./submodule")
set(CMAKE_CXX_STANDARD 17) # 设置std=c++11

# 设置debug或者release的编译参数
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fPIC")


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# collect all message to "SRC_LIST" <-- ./*.cpp
aux_source_directory(. SRC_LIST)

#分别指定编译include和链接link的搜索目录
include_directories(${SUB_MODULE_PATH})
link_directories(${SUB_MODULE_PATH})

#添加一个子模块，编译到lib目录下去
# add_subdirectory(${SUB_MODULE_PATH} lib) # add a submodule, and compile it to the "build/lib"
#设置编译目标
add_executable(ipc_benchmark ${SRC_LIST})
#添加编译目标依赖
target_link_libraries(ipc_benchmark 
                    glog 
                    pthread 
                    bfd 
                    dl
                    rt)
