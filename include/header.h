#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <regex>
#include <thread>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <sys/time.h>
#include <sys/timerfd.h>
#include "../protobuf/Login_Protocol.pb.h"
#include "../protobuf/Hall_Protocol.pb.h"
#include "../protobuf/Room_Protocol.pb.h"

#define LOGGERMODE_ON
#include "Log.h"

#include "Mysql_Server.h"

#define DEFAULT_TCP_PORT 2336

extern std::string LOCAL_IP;

struct Header
{
    int type = 0;
    int length = 0;
    Header(){};
    Header(int type) : type(type){};
};

void setnonblocking(int fd);

void addfd(int epollfd, int fd, bool block = true);

void delfd(int epollfd, int fd);

int get_new_socket(std::string IP, uint16_t socket_port, __socket_type protocol, sockaddr_in &sock_addr);
