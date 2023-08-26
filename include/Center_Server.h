#pragma once

#include "header.h"
#include "MsgNum.h"
#include "Room_Manager.h"
#include "Hall_Manager.h"
#include "Login_Manager.h"
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
    template <typename T>
    void SendTo_SendQueue(int socket, T &message)
    {
        GateWay_Server::Instance()->SendTo_SendQueue(socket, message);
    }
    template <typename T>
    void SendMutiTo_SendQueue(vector<int> Vec_socket, T &message)
    {
        GateWay_Server::Instance()->SendMutiTo_SendQueue(Vec_socket, message);
    }

    /* 以下为逻辑处理 */
public:
    int OnProcess(Socket_Message *msg);
    int OnProcess(Token_SocketMessage *TokenMsg);
    void Push_LoginUser(User_Info *userinfo);
    User_Info *Get_Userinfo(int socket_fd);
    // bool Check_Reconnect(Socket_Info *Sockinfo, string &token);

protected:
    void OnLogout(const int socket_fd, const Header header, const char *content);

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
