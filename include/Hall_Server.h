#pragma once

#include "header.h"
#include "NetworkStuct.h"
#include "Room_Process.h"

using namespace std;

#define NumOfHallThread 4
class Hall_Server
{
public:
    static Hall_Server *Instance()
    {
        static Hall_Server *m_Instance = new Hall_Server();
        return m_Instance;
    }

public:
    bool Init_Hall();
    int Run();
    void ThreadEnd();

    void Push_User(User_Info *info);

protected:
    void Recv_Process(); // 接收线程函数
    void Hall_Process(); // 处理线程函数
    void Send_Process(); // 发送线程函数

    int OnHallProcess(Socket_Message *msg);
    void Hall_Message(int socket, Header header, char *content);
    void Get_hall_info(int sock_accept);
    void Create_Room(int sock_accept);
    void Enter_Room(int sock_accept, Header header, char *content);

    template <typename T>
    int Get_Header_Type(T &message);
    template <typename T>
    void SendTo_SendQueue(int socket, T &message);

private:
    Hall_Server(){};

    User_Info *Get_Userinfo(int socket);

public:
    int Get_pipe() { return Hall_pipe[1]; };

private:
    // Epoll
    int Hall_epoll = epoll_create(100);
    epoll_event Hall_events[200];
    int Hall_pipe[2];

    // Process
    queue<Socket_Message *> RecvQueue; // 接收队列
    queue<Socket_Message *> SendQueue; // 发送队列

    mutex HallProcess_mtx;             // 唤醒处理线程的锁
    condition_variable HallProcess_cv; // 唤醒处理线程的条件变量
    mutex SendProcess_mtx;             // 唤醒发送线程的锁
    condition_variable SendProcess_cv; // 唤醒发送线程的条件变量

    mutex RecvQueue_mtx; // 接收队列的锁
    mutex SendQueue_mtx; // 发送队列的锁

    // Logic Process
    vector<User_Info *> user_list;
    // vector<Room_Process *> room_list;
    int room_counter;
    bool Hall_Process_stop = false;
};

extern vector<int> game_pipe_list;
