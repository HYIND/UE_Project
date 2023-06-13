#pragma once

#include "header.h"

using namespace std;

#define TokenSize 32
string Get_Token();
string EmptyToken();

struct Header
{
    int type = 0;
    int length = 0;
    Header() {}
    Header(int type) : type(type) {}
};

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

struct Token_SocketMessage
{
    string token;
    Socket_Message *msg = nullptr;

    Token_SocketMessage(int socket_fd) { msg = new Socket_Message(socket_fd); }
    void Delete_SocketMessage()
    {
        delete msg;
        msg = nullptr;
    }
    ~Token_SocketMessage()
    {
        if (msg)
        {
            delete msg;
            msg = nullptr;
        }
    }
};

struct RPC_Message
{
    // Request
    string Request_token;
    Header Request_header;
    char *Request_buffer = nullptr;

    // Response
    Header Response_header;
    char *Response_buffer = nullptr;

    // server
    sockaddr_in remote_addr;

    RPC_Message() {}
    RPC_Message(string token_in, sockaddr_in remote_addr_in) : Request_token(token_in), remote_addr(remote_addr_in) {}

    ~RPC_Message()
    {
        if (Request_buffer)
            delete Request_buffer;
        if (Response_buffer)
            delete Response_buffer;
    };

    template <typename T>
    void Fill_RequestInfo(T &proto_message)
    {
        Request_header.type = Get_Header_Type(proto_message);
        if (Request_header.type == 0)
            return;
        Request_header.length = proto_message.ByteSizeLong();

        Request_buffer = new char[Request_header.length + 1];
        proto_message.SerializeToArray(Request_buffer, Request_header.length);
    }
    template <typename T>
    void Fill_ResponseInfo(T &proto_message)
    {
        Response_header.type = Get_Header_Type(proto_message);
        if (Response_header.type == 0)
            return;
        Response_header.length = proto_message.ByteSizeLong();

        Response_buffer = new char[Response_header.length + 1];
        proto_message.SerializeToArray(Response_buffer, Response_header.length);
    }
    void Fill_RemoteAddrInfo(string &IP, int Port);
    bool GenerateRequestBuf(char *&buf_out, int *sum_bufferlen_out);
    bool GenerateResponseBuf(char *&buf_out, int *sum_bufferlen_out);
    bool ParseRequestBuf(const char *buf, const int len);
    bool ParseResponseBuf(const char *buf, const int len);
};

struct Epoll_Data
{
    int fd = 0;
    int wrongcount = 0;

    Epoll_Data(int fd_in) : fd(fd_in){};
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

    int Get_SocketFd() const { return tcp_fd; }
    string Get_IP() const { return inet_ntoa(tcp_addr.sin_addr); }
};

enum class USER_STATE
{
    Logining = 0,
    Hall,
    Room,
    Gaming,
    Disconnection
};
struct User_Info
{
    int id;
    string name;
    string token;
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

    User_Info(){};

    void SetSocketinfo_Move(Socket_Info *info) { sockinfo = info; }
    void SetSocketinfo_Copy(Socket_Info *info)
    {
        if (info)
            sockinfo = new Socket_Info(*info);
    }

    int Get_SocketFd() const { return sockinfo->Get_SocketFd(); }
    string Get_IP() const { return sockinfo->Get_IP(); }
    string Get_UserName() const { return name; }
    int Get_ID() const { return id; }
    string Get_Token() const { return token; }
};
