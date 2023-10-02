#include "header.h"
#include "UE_Server.h"
#include "GateWay_Server.h"
#include "Center_Server.h"
#include "Mysql_Server.h"
#include "RPC_Server.h"

using namespace std;

void sig_handler(int sig)
{
    LOGINFO("sig_handler ,recive sig : {}", sig);
    int save_errno = errno;
    int msg = sig;

    for (auto pipe : SigManager::Instance()->GetPipes())
    {
        send(pipe, (char *)&msg, 1, 0);
    }
    errno = save_errno;
}

void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}
int Init_Server(int argc, char *argv[])
{
    addsig(SIGINT);
    addsig(SIGTERM);
    signal(SIGPIPE, SIG_IGN);

    LOGINFO("Start Server!");

    string host = Config::Instance()->Read<string>("mysql_host", "127.0.0.1");
    int port = Config::Instance()->Read("mysql_port", 0);
    string database = Config::Instance()->Read<string>("mysql_database", "NULL");
    string user = Config::Instance()->Read<string>("mysql_user", "NULL");
    string password = Config::Instance()->Read<string>("mysql_password", "NULL");
    if (!Mysql_Server::Instance()->CreateConnect(host, port, database, user, password))
    {
        perror("Connect database error");
        return false;
    }

    sockaddr_in sockaddr;
    string IP = Config::Instance()->Read<string>("center_serverip", "0");
    int Port = Config::Instance()->Read("center_serverport", 0);
    int listen_socket = Get_newsocket(IP, Port, SOCK_STREAM, sockaddr);
    LOCAL_IP = IP;

    string RPC_IP = Config::Instance()->Read<string>("center_rpcip", "0");
    int RPC_Port = Config::Instance()->Read("center_rpcport", 0);

    if (listen_socket == -1)
    {
        perror("Create listen_socket error");
        return false;
    }

    Listen_Server::Instance()->Init_Listen(listen_socket);
    // RPC_Server::Instance()->Init(RPC_IP,RPC_Port);
    GateWay_Server::Instance()->Init_GateWayServer();

    thread T1(&Listen_Server::Run, Listen_Server::Instance());
    // thread T2(&RPC_Server::Start, RPC_Server::Instance());
    thread T3(&GateWay_Server::Run, GateWay_Server::Instance());

    // test
    // thread T5(
    //     []()
    //     {
    //         Game_Protobuf::NewGame_Request Request;
    //         RPC_Message Msg;
    //         RPC_Client::Instance()->Excute_RPCRequest(Request, Msg, RPC_GameServer);
    //     });

    T3.join();
    T1.join();

    return true;
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    Init_Server(argc, argv);

    return 0;
}
