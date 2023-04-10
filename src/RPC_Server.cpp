#include "RPC_Server.h"
#include "RPC_ClosureGuard.h"

void RPC_Server::Init()
{
    string IP = Config::Instance()->Read<string>("center_rpcip", "0");
    int Port = Config::Instance()->Read("center_rpcport", 0);
    Server_Socket = Get_newsocket(IP, Port, SOCK_DGRAM, Server_Addr);
    LOGINFO("RPC_Server::Init : IP : {},Port : {}", IP, Port);
}

void RPC_Server::Start()
{
    while (true)
    {
        RPC_Message RPC_Msg;
        socklen_t from_len = sizeof(RPC_Msg.remote_addr);
        char re_buf[4096];
        int re_num = recvfrom(Server_Socket, re_buf, 4095, 0, (struct sockaddr *)&RPC_Msg.remote_addr, &from_len);

        if (re_num < 0)
            continue;
        else if (re_num >= (int)(TokenSize + sizeof(Header)))
        {
            if (!RPC_Msg.ParseRequestBuf(re_buf, re_num))
                continue;

            if (!Center_Server::Instance()->CheckToken(RPC_Msg.Request_token))
                continue;

            RPC_Process(RPC_Msg);

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

int RPC_Server::RPC_Process(RPC_Message &Msg)
{
    switch (Msg.Request_header.type)
    {
    case Request_Login:
        RPC_Login(Msg);
        break;

    default:
        break;
    }

    return 1;
}

void RPC_Server::RPC_Login(RPC_Message &Msg)
{
    // Login_Protobuf::Login_Request Request;
    // Request.ParseFromArray(Msg.Request_buffer, Msg.Request_header.length);

    // string account = Request.logininfo().account();
    // string password = Request.logininfo().password();
    // User_Info *userinfo = nullptr;

    // Login_Protobuf::Login_Response Response;
    // Response.set_result((int)Login_Server::Instance()->Check_Login(account, password, &userinfo));

    // if (Response.result() == (int)Login_Request_Result::LoginSuccess)
    // {

    //     if (!userinfo)
    //         Response.set_result((int)Login_Request_Result::InnerError);
    //     else
    //     {
    //         userinfo->token = Get_Token();
    //         Center_Server::Instance()->Push_User(userinfo);

    //         Response.set_name(userinfo->Get_UserName());
    //         Response.set_result((int)Login_Request_Result::LoginSuccess);
    //         Response.set_token(userinfo->token);
    //     }
    // }
    // Msg.Fill_ResponseInfo(Response);
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