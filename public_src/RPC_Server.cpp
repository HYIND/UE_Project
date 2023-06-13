#include "RPC_Server.h"

void RPC_Server::Init()
{
    string IP = Config::Instance()->Read<string>("center_rpcip", "0");
    int Port = Config::Instance()->Read("center_rpcport", 0);
    Server_Socket = Get_newsocket(IP, Port, SOCK_DGRAM, Server_Addr);
    setnonblocking(Server_Socket);

    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, pipe) != -1);
    setnonblocking(pipe[1]);
    addfd(epoll, pipe[0]);

    LOGINFO("RPC_Server::Init : IP : {},Port : {}", IP, Port);
}

void RPC_Server::Start()
{
    stop = false;

    while (!stop)
    {
        int num = epoll_wait(epoll, events, 200, -1);
        if (num < 0 && (errno != EINTR))
        {
            perror("RPC_epoll failed!");
            break;
        }

        for (int i = 0; i < num; i++)
        {
            int socket_fd = ((Epoll_Data *)events[i].data.ptr)->fd;
            uint32_t event = events[i].events;
            if ((socket_fd == pipe[0]) && (event & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(pipe[0], signals, 1023, 0);
                if (ret == -1 || ret == 0)
                    continue;
                else
                {
                    for (i = 0; i < ret; i++)
                    {
                        switch (signals[i])
                        {
                        case SIGINT:
                        case SIGTERM:
                        {
                            stop = true;
                            break;
                        }
                        }
                    }
                }
            }
            else if (socket_fd == Server_Socket && (event & EPOLLIN))
            {
                while (true)
                {
                    RPC_Message RPC_Msg;
                    socklen_t from_len = sizeof(RPC_Msg.remote_addr);
                    char re_buf[4096];
                    int re_num = recvfrom(Server_Socket, re_buf, 4095, 0, (struct sockaddr *)&RPC_Msg.remote_addr, &from_len);

                    if (re_num < 0)
                        break;
                    else if (re_num >= (int)(TokenSize + sizeof(Header)))
                    {
                        if (!RPC_Msg.ParseRequestBuf(re_buf, re_num))
                            continue;

                        // if (!Center_Server::Instance()->CheckToken(RPC_Msg.Request_token))
                        //     continue;

                        OnProcess(RPC_Msg);

                        if (RPC_Msg.Response_header.type != 0 && RPC_Msg.Response_buffer)
                        {
                            char *sum_sendbuf = nullptr;
                            int sum_sendlen = 0;
                            if (RPC_Msg.GenerateResponseBuf(sum_sendbuf, &sum_sendlen) != false)
                                sendto(Server_Socket, sum_sendbuf, sum_sendlen, 0, (struct sockaddr *)&RPC_Msg.remote_addr, from_len);
                            if (sum_sendbuf)
                                delete sum_sendbuf;
                        }
                    }
                }
            }
        }
    }
    close(epoll);
}

int RPC_Server::OnProcess(RPC_Message &Msg)
{
    // switch (Msg.Request_header.type)
    // {
    // case Request_Login:
    //     _Dispatche(Msg, &Login_Server::OnLogin, Login_Server::Instance());
    //     break;

    // default:
    //     break;
    // }

    return 1;
}

int RPC_Server::Process(RPC_Message &Msg)
{
    int result = OnProcess(Msg);
    return result;
}

template <class _Request, class _Response>
void RPC_Server::_Dispatche(RPC_Message &Msg, void (*Fun)(_Request &, _Response &))
{
    _Request Request;
    _Response Response;
    Request.ParseFromArray(Msg.Request_buffer, Msg.Request_header.length);

    *Fun(Request, Response);

    Msg.Fill_ResponseInfo(Response);
}

template <class _Request, class _Response, class _Instance>
void RPC_Server::_Dispatche(RPC_Message &Msg, void (_Instance::*Fun)(_Request &, _Response &), _Instance *Instance)
{
    _Request Request;
    _Response Response;
    Request.ParseFromArray(Msg.Request_buffer, Msg.Request_header.length);

    if (Instance != nullptr)
        (Instance->*Fun)(Request, Response);

    Msg.Fill_ResponseInfo(Response);
}

bool RPC_Client::Excute_SocketRequest(RPC_Message &Msg, char *re_buf, int *re_num)
{
    assert(re_buf != nullptr && re_num != nullptr);

    timeval tv = {5, 0};
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(timeval));

    int sum_sendlen = 0;
    char *sum_Request_buf;
    Msg.GenerateRequestBuf(sum_Request_buf, &sum_sendlen);
    int send_num = sendto(socket_fd, sum_Request_buf, sum_sendlen, 0, (struct sockaddr *)&Msg.remote_addr, sizeof(Msg.remote_addr));
    if (sum_Request_buf)
        delete sum_Request_buf;
    if (send_num <= 0)
    {
        perror("RPC_Client::Excute_SocketRequest :Request wrong!");
        LOGERROR("RPC_Client::Excute_SocketRequest :Request wrong!");
        close(socket_fd);
        return false;
    }

    socklen_t from_len = sizeof(Msg.remote_addr);
    *re_num = recvfrom(socket_fd, re_buf, 4095, 0, (struct sockaddr *)&Msg.remote_addr, &from_len);

    if (*re_num <= 0)
    {
        perror("RPC_Client::Excute_SocketRequest :No Response!");
        LOGERROR("RPC_Client::Excute_SocketRequest :No Response!");
        close(socket_fd);
        return false;
    }

    close(socket_fd);
    return true;
}