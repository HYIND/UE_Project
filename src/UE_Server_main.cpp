#include "header.h"
#include "UE_Server.h"
#include "Hall_Server.h"
#include "Mysql_Server.h"
#include <net/if.h>
#include <sys/ioctl.h>
using namespace std;

int get_local_ip(const char *eth_inf, char *out)
{
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    // if error: No such device
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
    {
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return -1;
    }

    strcpy(out, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    close(sd);
    return 0;
}

void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(Listen_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    send(Login_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
    send(Hall_Server::Instance()->Get_pipe(), (char *)&msg, 1, 0);
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
    if (argc > 1)
    {
        listen_socket = get_new_socket(argv[1], DEFAULT_TCP_PORT, SOCK_STREAM, sockaddr);
        LOCAL_IP = argv[1];
    }
    else
    {
        char IP[20] = {'\0'};
        get_local_ip("eth0", IP);
        listen_socket = get_new_socket(IP, DEFAULT_TCP_PORT, SOCK_STREAM, sockaddr);
        LOCAL_IP = IP;
    }

    if (listen_socket == -1)
    {
        perror("Create listen_socket error");
        return false;
    }

    Listen_Server::Instance()->Init_Listen(listen_socket);
    Login_Server::Instance()->Init_Login();
    Hall_Server::Instance()->Init_Hall();

    addsig(SIGINT);
    addsig(SIGTERM);
    signal(SIGPIPE, SIG_IGN);

    thread T1(&Hall_Server::Run, Hall_Server::Instance());
    thread T2(&Login_Server::Run, Login_Server::Instance());
    thread T3(&Listen_Server::Run, Listen_Server::Instance());

    // test
    // thread T4(
    //     []()
    //     {
    //         socket_message *msg = new socket_message(0);
    //         msg->header.type = 700;
    //         Login_Server::Instance()->OnLoginProcess(msg);
    //     }
    //     );

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
