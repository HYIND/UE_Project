cmake_minimum_required (VERSION 3.16)

project(UE_Server C CXX)

# C++20
set(CMAKE_CXX_STANDARD 20)

ADD_SUBDIRECTORY(protobuf)
ADD_SUBDIRECTORY(public_src)

include_directories(include)
include_directories(protobuf)
include_directories(spdlog)
include_directories(public_include)

aux_source_directory(src DIR_SRCS)

add_executable(${PROJECT_NAME} ${DIR_SRCS})

# set(CMAKE_BUILD_TYPE "Debug")

target_link_libraries(${PROJECT_NAME} public proto)

set(EXECUTABLE_OUTPUT_PATH ../bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lfmt -lrabbitmq")
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

