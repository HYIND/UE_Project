#include "header.h"
#include "UE_Server.h"
#include "GateWay_Server.h"
#include "Center_Server.h"
#include "Mysql_Server.h"
#include "RPC_Server.h"

using namespace std;

int Init_Server(int argc, char *argv[])
{
    LOGINFO("Start Server!");
    if (!Mysql_Server::Instance()->CreateConnect())
    {
        perror("Contect database error");
        return false;
    }

    sockaddr_in sockaddr;
    string IP = Config::Instance()->Read<string>("center_serverip", "0");
    int Port = Config::Instance()->Read("center_serverport", 0);
    int listen_socket = Get_newsocket(IP, Port, SOCK_STREAM, sockaddr);
    LOCAL_IP = IP;

    if (listen_socket == -1)
    {
        perror("Create listen_socket error");
        return false;
    }

    Listen_Server::Instance()->Init_Listen(listen_socket);
    GateWay_Server::Instance()->Init_GateWayServer();
    // Login_Server::Instance()->Init_Login();
    // Center_Server::Instance()->Init_CenterServer();
    // RPC_Server::Instance()->Init();

    addsig(SIGINT);
    addsig(SIGTERM);
    signal(SIGPIPE, SIG_IGN);

    // thread T1(&Center_Server::Run, Center_Server::Instance());
    // thread T2(&Login_Server::Run, Login_Server::Instance());
    thread T3(&Listen_Server::Run, Listen_Server::Instance());
    // thread T4(&RPC_Server::Start, RPC_Server::Instance());
    thread T5(&GateWay_Server::Run, GateWay_Server::Instance());

    // test
    // thread T5(
    //     []()
    //     {
    //         Game_Protobuf::NewGame_Request Request;
    //         RPC_Message Msg;
    //         RPC_Client::Instance()->Excute_RPCRequest(Request, Msg, RPC_GameServer);
    //     });

    T5.join();
    // T4.join();
    T3.join();
    // T2.join();
    // T1.join();

    return true;
}

#include "MessageQueue.h"
void consume()
{
    Consumer_MQ rabbitmq;
    if (!rabbitmq.Consumer_Connect())
        return;
    if (!rabbitmq.Consumer_BuildQueue())
        return;
    char *msg = nullptr;
    size_t length = 0;
    while (rabbitmq.Consumer(msg, length) > 0)
    {
        char m[20];
        memcpy(m, msg, length);
        cout << "consume , recv length :" << length << ",msg:" << msg << endl;
        delete (msg);
        msg = nullptr;
        sleep(1);
    }
    rabbitmq.Consumer_Close();
}

void produce()
{
    Producer_MQ rabbitmq;
    if (!rabbitmq.Producer_Connect())
        return;
    for (int i = 0; i < 5;)
    {
        char msg[] = "asjdha\0sdasd";
        msg[0] += i;
        int length = sizeof(msg);
        rabbitmq.Producer_Publish(msg, length);
        cout << "consume , send length :" << length << ",msg:" << msg << endl;

        // sleep(1);
    }
    rabbitmq.Producer_Close();
}

void messagequeuetest()
{
    thread T1(consume);
    thread T2(produce);
    T1.join();
    T2.join();
}

#include "ThreadPool.h"
void threadpooltest()
{

    ThreadPool pool;
    pool.start();

    User_Info *user = new User_Info();
    sockaddr_in tcp_addr_in;
    user->sockinfo = new Socket_Info(10,tcp_addr_in);

    pool.submit(&Center_Server::Push_LoginUser, Center_Server::Instance(), user);   //类成员函数
    pool.submit(messagequeuetest);  //普通函数

    pool.stop();
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // Init_Server(argc, argv);

    threadpooltest();

    return 0;
}
