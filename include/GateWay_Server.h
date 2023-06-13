#pragma once
#include "header.h"
#include "RateLimiter.h"

class Center_Server;
class Login_Server;

class GateWay_Server
{
    /* 以下为基础服务框架 */
public:
    static GateWay_Server *Instance()
    {
        static GateWay_Server *m_Instance = new GateWay_Server();
        return m_Instance;
    }

private:
    GateWay_Server(){};

public:
    bool Init_GateWayServer();
    int Run();
    void ThreadEnd();

    int Get_pipe() { return Pipe[1]; };

protected:
    void Recv_Process();           // 接收线程函数
    void GateWay_Process();        // 处理线程函数
    void Send_Process();           // 发送线程函数
    void HeartBeatCheck_Process(); // 心跳检测函数

private:
    // Epoll
    int Pipe[2];
    int Epoll = epoll_create(100);
    epoll_event Events[100];

    // Process
    queue<Token_SocketMessage *> RecvQueue; // 接收队列
    queue<Socket_Message *> SendQueue;      // 发送队列

    mutex GateWayProcess_mtx;             // 唤醒处理线程的锁
    condition_variable GateWayProcess_cv; // 唤醒处理线程的条件变量
    mutex SendProcess_mtx;                // 唤醒发送线程的锁
    condition_variable SendProcess_cv;    // 唤醒发送线程的条件变量

    mutex RecvQueue_mtx; // 接收队列的锁
    mutex SendQueue_mtx; // 发送队列的锁

    bool Process_stop = false;

    /* 以下为逻辑处理 */
public:
    User_Info *GetUser(int fd);
    void Push_NewUser(int socket_fd, sockaddr_in &tcp_addr);
    void RemoveUser(int fd);

protected:
    int OnProcess(Token_SocketMessage *Tokenmsg);

private:
    // Logic Process
    vector<User_Info *> AllUser_list;
    vector<User_Info *> wait_list;
    map<int, int> HeartBeat_map; //<fd->count>

public:
    // Template

    // template <typename T>
    // int Get_Header_Type(T &message);

    template <typename T>
    void SendTo_SendQueue(int socket, T &message)
    {
        Header header;
        header.type = Get_Header_Type(message);
        if (header.type == 0)
            return;
        header.length = message.ByteSizeLong();

        Socket_Message *msg = new Socket_Message(socket, header);
        msg->content = new char[header.length + 1];
        message.SerializeToArray(msg->content, header.length);

        SendQueue.emplace(msg);      // 投递消息
        SendProcess_cv.notify_one(); // 唤醒一个发送线程
    }

    template <typename T>
    void SendMutiTo_SendQueue(vector<int> Vec_socket, T &message)
    {
        Header header;
        header.type = Get_Header_Type(message);
        if (header.type == 0)
            return;
        header.length = message.ByteSizeLong();

        char *buf = new char[header.length + 1];
        message.SerializeToArray(buf, header.length);

        vector<Socket_Message *> Vec_msg;
        for (auto socket : Vec_socket)
        {
            Socket_Message *msg = new Socket_Message(socket, header);
            msg->content = new char[header.length + 1];
            memcpy(msg->content, buf, header.length);
            Vec_msg.emplace_back(msg);
        }

        int count = 0;
        for (auto &msg : Vec_msg)
        {
            SendQueue.emplace(msg); // 投递消息
            count++;
            if (count % 5 == 0)
                SendProcess_cv.notify_one();
        }
        SendProcess_cv.notify_one();
    }
};