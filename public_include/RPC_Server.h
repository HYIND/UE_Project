#pragma once
#include "header.h"

using namespace std;

class RPC_Server
{
public:
    static RPC_Server *Instance()
    {
        static RPC_Server *m_Instance = new RPC_Server();
        return m_Instance;
    }

private:
    RPC_Server() {}

public:
    void Init();
    void Start();
    int Get_pipe() { return pipe[1]; };

protected:
    int Process(RPC_Message &msg);
    virtual int OnProcess(RPC_Message &msg);

    template <class _Request, class _Response>
    void _Dispatche(RPC_Message &Msg, void (*Fun)(_Request &, _Response &));
    template <class _Request, class _Response, class _Instance>
    void _Dispatche(RPC_Message &Msg, void (_Instance::*Fun)(_Request &, _Response &), _Instance *instance);

    // void RPC_OpenGame(RPC_Message &msg);
    void RPC_Login(RPC_Message &msg);

private:
    // Stopflag
    bool stop = false;

    // ListenSocket
    int Server_Socket;
    sockaddr_in Server_Addr;

    // Epoll
    int pipe[2];
    int epoll = epoll_create(100);
    epoll_event events[100];
};

enum RCP_DstServer
{
    RPC_CenterServer = 1,
    RPC_GameServer = 1
};

class RPC_Client
{
public:
    static RPC_Client *Instance()
    {
        static RPC_Client *m_Instance = new RPC_Client();
        return m_Instance;
    }

private:
    RPC_Client() {}

protected:
    bool Excute_SocketRequest(RPC_Message &Msg, char *re_buf, int *re_num);

public:
    template <typename T1, typename T2>
    bool Excute_RPCRequest(T1 &Request, T2 &Response, RCP_DstServer Dst_Server, int trycount = 1)
    {
        RPC_Message Msg;
        string IP;
        int Port = 0;
        switch (Dst_Server)
        {
        case RPC_GameServer:
            IP = Config::Instance()->Read<string>("game_rpcip", "0");
            Port = Config::Instance()->Read("game_rpcport", 0);
            break;

        default:
            LOGERROR("RPC_Client::SendRPC_Request: Dst_Server wrong!");
            return false;
        }
        if (IP == "0" || Port == 0)
        {
            LOGERROR("RPC_Client::SendRPC_Request: Server {} config wrong!", (int)Dst_Server);
            return false;
        }

        Msg.Fill_RemoteAddrInfo(IP, Port);
        Msg.Fill_RequestInfo(Request);

        char re_buf[4096];
        int re_num = 0;

        while (!Excute_SocketRequest(Msg, re_buf, &re_num))
        {
            trycount--;
            if (trycount <= 0)
                return false;
        }

        if (!Msg.ParseResponseBuf(re_buf, re_num))
            return false;

        Response.ParseFromArray(Msg.Response_buffer, Msg.Response_header.length);
        return true;
    }
};
