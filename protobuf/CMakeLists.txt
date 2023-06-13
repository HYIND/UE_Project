cmake_minimum_required (VERSION 3.16)

project(UE_Server_Proto C CXX)

include_directories(./)
include_directories(${Protobuf_INCLUDE_DIRS})

aux_source_directory(. DIR_PROTO)

find_package(Protobuf REQUIRED)

ADD_LIBRARY(proto SHARED ${DIR_PROTO})

target_link_libraries(proto ${Protobuf_LIBRARIES})

SET(LIBRARY_OUTPUT_PATH ../../bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lfmt -lrabbitmq")

