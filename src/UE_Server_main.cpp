#include "header.h"
#include "UE_Server.h"
#include "Center_Server.h"
#include "Mysql_Server.h"
#include "RPC_Server.h"
#include "event.h"
using namespace std;

void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(Listen_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    send(Login_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    send(Center_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    errno = save_errno;
}

void addsig(int sig)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    // sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

int Init_Server(int argc, char *argv[])
{
    LOGINFO("Start Server!");
    if (!Mysql_Server::Instance()->CreateConnect())
    {
        perror("Contect database error");
        return false;
    }
    sockaddr_in sockaddr;
    int listen_socket;

    string IP = Config::Instance()->Read<string>("center_serverip", "0");
    int Port = Config::Instance()->Read("center_serverport", 0);
    listen_socket = Get_newsocket(IP, Port, SOCK_STREAM, sockaddr);
    LOCAL_IP = IP;

    if (listen_socket == -1)
    {
        perror("Create listen_socket error");
        return false;
    }

    Listen_Server::Instance()->Init_Listen(listen_socket);
    Login_Server::Instance()->Init_Login();
    Center_Server::Instance()->Init_CenterServer();
    RPC_Server::Instance()->Init();

    addsig(SIGINT);
    addsig(SIGTERM);
    signal(SIGPIPE, SIG_IGN);

    thread T1(&Center_Server::Run, Center_Server::Instance());
    thread T2(&Login_Server::Run, Login_Server::Instance());
    thread T3(&Listen_Server::Run, Listen_Server::Instance());
    thread T4(&RPC_Server::Start, RPC_Server::Instance());

    // test
    // thread T5(
    //     []()
    //     {
    //         Game_Protobuf::NewGame_Request Request;
    //         RPC_Message Msg;
    //         RPC_Client::Instance()->Excute_RPCRequest(Request, Msg, RPC_GameServer);
    //     });

    T4.join();
    T3.join();
    T2.join();
    T1.join();

    return true;
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    Init_Server(argc, argv);

    return 0;
}
