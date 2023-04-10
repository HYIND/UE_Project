#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
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
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <functional>
#include <random>
#include <net/if.h>
#include <sys/ioctl.h>

#include "Config.h"

#define LOGGERMODE_ON
#include "Log.h"

#include "Mysql_Server.h"
#include "MsgNum.h"
#include "NetworkBase.h"

extern std::string LOCAL_IP;

void setnonblocking(int fd);

void addfd(int epollfd, int fd, bool block = true);

void delfd(int epollfd, int fd);

int get_local_ip(const char *eth_inf, char *out);
int Get_newsocket(std::string IP, uint16_t socket_port, __socket_type protocol, sockaddr_in &sock_addr);
