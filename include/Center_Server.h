#pragma once

#include "header.h"
#include "MsgNum.h"
#include "Room_Manager.h"
#include "Hall_Manager.h"
#include "Login_Server.h"
#include "GateWay_Server.h"
#include "RateLimiter.h"
using namespace std;

class GateWay_Server;

#define NumOfProcessThread 4
class Center_Server
{

public:
    static Center_Server *Instance()
    {
        static Center_Server *m_Instance = new Center_Server();
        return m_Instance;
    }

private:
    Center_Server(){};

public:
    // bool Init_CenterServer();
    // int Run();
    // void ThreadEnd();

    // int Get_pipe() { return pipe[1]; };

    template <typename T>
    void SendTo_SendQueue(int socket, T &message)
    {
        // Header header;
        // header.type = Get_Header_Type(message);
        // if (header.type == 0)
        //     return;
        // header.length = message.ByteSizeLong();

        // Socket_Message *msg = new Socket_Message(socket, header);
        // msg->content = new char[header.length + 1];
        // message.SerializeToArray(msg->content, header.length);

        // SendQueue.emplace(msg);      // 投递消息
        // SendProcess_cv.notify_one(); // 唤醒一个发送线程
        GateWay_Server::Instance()->SendTo_SendQueue(socket, message);
    }
    template <typename T>
    void SendMutiTo_SendQueue(vector<int> Vec_socket, T &message)
    {
        // Header header;
        // header.type = Get_Header_Type(message);
        // if (header.type == 0)
        //     return;
        // header.length = message.ByteSizeLong();

        // char *buf = new char[header.length + 1];
        // message.SerializeToArray(buf, header.length);

        // vector<Socket_Message *> Vec_msg;
        // for (auto socket : Vec_socket)
        // {
        //     Socket_Message *msg = new Socket_Message(socket, header);
        //     msg->content = new char[header.length + 1];
        //     memcpy(msg->content, buf, header.length);
        //     Vec_msg.emplace_back(msg);
        // }

        // int count = 0;
        // for (auto &msg : Vec_msg)
        // {
        //     SendQueue.emplace(msg); // 投递消息
        //     count++;
        //     if (count % 10 == 0)
        //         SendProcess_cv.notify_one();
        // }
        // SendProcess_cv.notify_one();

        GateWay_Server::Instance()->SendMutiTo_SendQueue(Vec_socket, message);
    }

// protected:
//     void Recv_Process();           // 接收线程函数
//     void Center_Process();         // 处理线程函数
//     void Send_Process();           // 发送线程函数
//     void HeartBeatCheck_Process(); // 心跳检测函数

private:
    // Epoll
    // int epoll = epoll_create(100);
    // epoll_event events[200];
    // int pipe[2];

    // Mutex
    // queue<Socket_Message *> RecvQueue; // 接收队列
    // queue<Socket_Message *> SendQueue; // 发送队列

    // mutex HallProcess_mtx;             // 唤醒处理线程的锁
    // condition_variable HallProcess_cv; // 唤醒处理线程的条件变量
    // mutex SendProcess_mtx;             // 唤醒发送线程的锁
    // condition_variable SendProcess_cv; // 唤醒发送线程的条件变量

    // mutex RecvQueue_mtx; // 接收队列的锁
    // mutex SendQueue_mtx; // 发送队列的锁

    // bool Process_stop = false;

    /* 以下为逻辑处理 */
public:
    int OnProcess(Socket_Message *msg);
    int OnProcess(Token_SocketMessage *TokenMsg);
    // void Push_User(User_Info *userinfo);
    void Push_LoginUser(User_Info *userinfo);
    User_Info *Get_Userinfo(int socket_fd);
    // bool Check_Reconnect(Socket_Info *Sockinfo, string &token);

protected:
    void OnLogout(const int socket_fd, const Header header, const char *content);
    // void OnPing(const int socket_fd, const Header header, const char *content);

public:
    vector<User_Info *> Get_user_list() { return user_list; };
    void RemoveUser(int fd); // 移除Fd，若有关联用户则用户下线
    bool CheckToken(string &token);

protected:
    // void MoveToWaitList(int fd); // 移动fd的相关用户至等待列表，等待重连
private:
    // struct HeartCount
    // {
    //     int count = 0;
    //     User_Info *userdata;
    //     HeartCount &operator=(const HeartCount &other)
    //     {
    //         count = other.count;
    //         userdata = other.userdata;
    //         return *this;
    //     }
    //     HeartCount() {}
    //     HeartCount(User_Info *user) : userdata(user) {}
    // };

    vector<User_Info *> user_list;
    // vector<HeartCount> wait_list;
    map<string, User_Info *> Token_map; //<token->userinfo>
    // map<int, HeartCount> HeartBeat_map; //<socket->userinfo>
};
