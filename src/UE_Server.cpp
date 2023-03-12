#include "UE_Server.h"

bool Listen_Server::Init_Listen(int socket)
{
    Listen_Socket = socket;
    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, Listen_pipe) != -1);
    setnonblocking(Listen_pipe[1]);
    addfd(Listen_epoll, Listen_pipe[0]);

    return true;
}
int Listen_Server::Run()
{
    thread listen_thread(&Listen_Server::Listen_Process, this);
    listen_thread.join();

    ThreadEnd();
    LOGINFO("Listen_Server::Run ,Listen_Server Close!");
    return 1;
}
void Listen_Server::ThreadEnd()
{
    close(Listen_pipe[0]);
    close(Listen_pipe[1]);
}
int Listen_Server::Listen_Process()
{
    if (Listen_Socket == 0)
    {
        LOGINFO("Listen_Server::Run  fail cause Listen_Socket not initialized");
        return -1;
    }
    addfd(Listen_epoll, Listen_Socket);
    int ret = listen(Listen_Socket, 10);
    if (ret < 0)
    {
        perror("listen socket error");
        return -1;
    }

    // int timefd = timerfd_create(CLOCK_MONOTONIC, 0);
    // itimerspec timer;

    // timeval now;
    // gettimeofday(&now, NULL);
    // timer.it_value.tv_sec =10;
    // timer.it_value.tv_nsec = 0;
    // timer.it_interval.tv_sec = 5;
    // timer.it_interval.tv_nsec = 0;
    // timerfd_settime(timefd, TFD_TIMER_ABSTIME, &timer, NULL);
    // addfd(listen_epoll, timefd, true);

    int stop = false;
    while (!stop)
    {
        int number = epoll_wait(Listen_epoll, Listen_events, 200, -1);
        if (number < 0 && (errno != EINTR))
        {
            cout << "listen_epoll failure\n";
            break;
        }
        for (int i = 0; i < number; i++)
        {
            // if ((listen_events[i].data.fd == timefd) && (listen_events[i].events & EPOLLIN))
            // {
            //     cout << "timer out!\n";
            //     uint64_t exp = 0;
            //     int ret=read(timefd, &exp, sizeof(uint64_t));
            //     cout<<exp;
            //     // addfd(listen_epoll,timefd,false);
            // }
            if ((Listen_events[i].data.fd == Listen_pipe[0]) && (Listen_events[i].events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(Listen_pipe[0], signals, 1023, 0);
                if (ret == -1 || ret == 0)
                    continue;
                else
                {
                    for (int i = 0; i < ret; i++)
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
            else if (Listen_events[i].events & EPOLLIN)
            {
                sockaddr_in client;
                socklen_t length = sizeof(client);
                int socket_fd = accept(Listen_Socket, (struct sockaddr *)&client, &length);
                if (socket_fd != -1)
                {
                    Login_Server::Instance()->Push_Fd(socket_fd, client);
                    LOGINFO("user connect : address : {}:{},", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                }
                else
                    cout << "socket accept fail!\n";
            }
        }
    }
    // close(timefd);
    close(Listen_epoll);
    close(Listen_Socket);

    return 1;
}