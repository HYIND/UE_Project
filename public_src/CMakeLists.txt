cmake_minimum_required (VERSION 3.16)

project(UE_Server_Shared C CXX)

# include_directories(../protobuf)
include_directories(../spdlog)
include_directories(../public_include)
include_directories(.)

# aux_source_directory(../protobuf DIR_PROTO)
aux_source_directory(. DIR_PUBLIC)

ADD_LIBRARY(public SHARED ${DIR_PUBLIC})

target_link_libraries(public libmysqlclient.so)
target_link_libraries(public librabbitmq.so)

SET(LIBRARY_OUTPUT_PATH ../../bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lfmt -lrabbitmq")