#pragma once

#include "header.h"
#include "Login_Manager.h"
#include "GateWay_Server.h"

using namespace std;

class Listen_Server
{
public:
    static Listen_Server *Instance()
    {
        static Listen_Server *m_Instance = new Listen_Server();
        return m_Instance;
    }

public:
    bool Init_Listen(int Socket);
    int Run();
    void ThreadEnd();

    int Listen_Process();

private:
    Listen_Server(){};

public:
    int Get_pipe() { return Listen_pipe[1]; };

private:
    int Listen_pipe[2];
    int Listen_Socket = 0;
    int Listen_epoll = epoll_create(100);
    epoll_event Listen_events[100];
};