cmake_minimum_required (VERSION 3.16)

project(UE_Server C CXX)

# C++20
set(CMAKE_CXX_STANDARD 20)

ADD_SUBDIRECTORY(protobuf)
add_library(public SHARED IMPORTED)
set_target_properties(public PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/bin/libpublic.so)

include_directories(include)
include_directories(protobuf)
include_directories(ThirdParty/public_include)
include_directories(ThirdParty/Log)

aux_source_directory(src DIR_SRCS)

add_executable(${PROJECT_NAME} ${DIR_SRCS})

# set(CMAKE_BUILD_TYPE "Debug")

target_link_libraries(${PROJECT_NAME} libmysqlclient.so)
target_link_libraries(${PROJECT_NAME} librabbitmq.so)
target_link_libraries(${PROJECT_NAME} libredis++.so)
target_link_libraries(${PROJECT_NAME} public proto)


set(EXECUTABLE_OUTPUT_PATH ../bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lfmt -lrabbitmq")
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

