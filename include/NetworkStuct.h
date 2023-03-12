#pragma once

#include "header.h"

struct Socket_Message
{
    int socket_fd;
    Header header;
    char *content = nullptr;

    Socket_Message(int socket) : socket_fd(socket), content(nullptr) {}
    Socket_Message(int socket, Header header) : socket_fd(socket), header(header), content(nullptr) {}
    ~Socket_Message()
    {
        if (content)
            delete content;
    }

    void SetHeader(){};
    void SetContent(){};
};

struct Socket_Info
{
    int tcp_fd = -1;
    sockaddr_in tcp_addr;

    Socket_Info(int socket_fd, sockaddr_in &tcp_addr_in)
    {
        tcp_fd = socket_fd;
        tcp_addr = tcp_addr_in;
    }

    int Get_SocketFd() { return tcp_fd; }
    string Get_IP() { return inet_ntoa(tcp_addr.sin_addr); }
};

enum class USER_STATE
{
    Login = 0,
    Hall,
    Room,
    Gaming,
    Disconnection
};
struct User_Info
{
    int id;
    string name;
    USER_STATE states;
    Socket_Info *sockinfo;

    User_Info(int id, string &strname, Socket_Info *info)
        : id(id), name(strname)
    {
        if (info)
            sockinfo = info;
    }

    User_Info(int id, string &strname, int socket_fd, sockaddr_in &tcp_addr_in)
        : id(id), name(strname)
    {
        sockinfo = new Socket_Info(socket_fd, tcp_addr_in);
    }

    User_Info(int id, string &strname)
        : id(id), name(strname){};
    User_Info(int id, string &&strname)
        : id(id), name(strname){};

    void SetSocketinfo_Move(Socket_Info *info) { sockinfo = info; }
    void SetSocketinfo_Copy(Socket_Info *info)
    {
        if (info)
            sockinfo = new Socket_Info(*info);
    }

    int Get_SocketFd() { return sockinfo->Get_SocketFd(); }
    string Get_IP() { return sockinfo->Get_IP(); }
    string Get_UserName() { return name; }
    int Get_ID() { return id; }
};

class Login_Server;
class Hall_Server;