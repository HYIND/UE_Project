#include "GateWay_Server.h"
#include "Center_Server.h"
#include "Login_Server.h"

bool GateWay_Server::Init_GateWayServer()
{
    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, Pipe) != -1);
    setnonblocking(Pipe[1]);
    addfd(Epoll, Pipe[0]);
    SigManager::Instance()->AddPipe(Pipe[1]);

    return true;
}
int GateWay_Server::Run()
{
    Process_stop = false;
    thread heartcheck_thread(&GateWay_Server::HeartBeatCheck_Process, this); // 心跳
    thread send_thread(&GateWay_Server::Send_Process, this);                 // 接收
    Pool.start();                                                            // 处理
    thread recv_thread(&GateWay_Server::Recv_Process, this);                 // 发送
    LOGINFO("GateWay_Server::Run ,GateWay_Server Start!");

    recv_thread.join();
    Pool.stop();
    send_thread.join();
    heartcheck_thread.join();

    ThreadEnd();
    LOGINFO("GateWay_Server::Run ,GateWay_Server Stop!");
    return 1;
}
void GateWay_Server::ThreadEnd()
{
    close(Pipe[0]);
    close(Pipe[1]);
    SigManager::Instance()->DelPipe(Pipe[1]);
}

void GateWay_Server::Recv_Process()
{
    char buffer[1024];
    memset(buffer, '\0', 1024);
    while (!Process_stop)
    {
        int num = epoll_wait(Epoll, Events, 200, -1);
        if (num < 0 && (errno != EINTR))
        {
            perror("Hall_epoll failed!");
            break;
        }

        for (int i = 0; i < num; i++)
        {
            int socket_fd = ((Epoll_Data *)Events[i].data.ptr)->fd;
            uint32_t event = Events[i].events;
            if ((socket_fd == Pipe[0]) && (event & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(Pipe[0], signals, 1023, 0);
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
                            Process_stop = true;
                            break;
                        }
                        }
                    }
                }
            }
            else if (event & EPOLLRDHUP)
            {
                RemoveUser(socket_fd);
            }
            else if (event & EPOLLIN)
            {
                int re_num = recv(socket_fd, buffer, TokenSize + sizeof(Header), 0);
                while (re_num > 0)
                {
                    int count = 0;
                    Token_SocketMessage *Token_Msg = new Token_SocketMessage(socket_fd);

                    // 获取token
                    char token_ch[TokenSize];
                    memcpy(token_ch, buffer, TokenSize);
                    Token_Msg->token.assign(token_ch, TokenSize);

                    // 获取头
                    memcpy(&Token_Msg->msg->header, buffer + TokenSize, sizeof(Header));

                    // 获取内容（可能为空）
                    if (Token_Msg->msg->header.length > 0)
                    {
                        Token_Msg->msg->content = new char[Token_Msg->msg->header.length + 1];
                        re_num = recv(socket_fd, Token_Msg->msg->content, Token_Msg->msg->header.length, 0);
                    }

                    HeartBeat_map[socket_fd] = 0;

                    if ((Token_Msg->msg->header.type < Login_MinNum || Token_Msg->msg->header.type > Login_MaxNum) && Token_Msg->msg->header.type != Heart_Package && !Center_Server::Instance()->CheckToken(Token_Msg->token))
                    {
                        ((Epoll_Data *)Events[i].data.ptr)->wrongcount++;
                        if (((Epoll_Data *)Events[i].data.ptr)->wrongcount >= 5)
                        {
                            RemoveUser(socket_fd);
                        }
                        delete (Token_Msg);
                        // Token_Msg.Delete_SocketMessage();
                    }
                    else
                    {

                        if (!RateLimiter_Manager::Instance()->TryPass(socket_fd) || Token_Msg->msg->header.type == Heart_Package)
                        {
                            delete (Token_Msg);
                            // Token_Msg.Delete_SocketMessage();
                        }
                        else
                        {
                            Pool.submit(&GateWay_Server::OnProcess, this, Token_Msg);
                            // unique_lock<mutex> Queuelck(RecvQueue_mtx);
                            // RecvQueue.enqueue(Token_Msg); // 投递消息
                            // Queuelck.release()->unlock();
                            // GateWayProcess_cv.notify_one(); // 唤醒其中一个处理线程
                        }
                    }

                    if (++count == 5) // 计数，防止持续占用
                        break;

                    re_num = recv(socket_fd, buffer, TokenSize + sizeof(Header), 0);
                }
                int save_errno = errno;
                if (re_num <= 0)
                {
                    if (errno != EAGAIN && errno != EINTR)
                    {
                        // 处理错误
                        RemoveUser(socket_fd);
                    }
                }
            }
        }
    }
    // GateWayProcess_cv.notify_all();
    SendProcess_cv.notify_all();
    close(Epoll);
}
void GateWay_Server::Send_Process()
{
    Socket_Message *Send_Message = nullptr;
    while (!Process_stop)
    {
        unique_lock<mutex> Sendlck(SendProcess_mtx);
        SendProcess_cv.wait(Sendlck);
        Sendlck.release()->unlock();

        while (!SendQueue.empty())
        {
            // GetMessage
            SendQueue.dequeue(Send_Message);

            if (!Send_Message)
                continue;

            // SendMessage
            int sendlen = sizeof(Header) + Send_Message->header.length;

            char *sendbuf = new char[sendlen];
            memset(sendbuf, '\0', sendlen);
            memcpy(sendbuf, &(Send_Message->header), sizeof(Header));
            memcpy(sendbuf + sizeof(Header), Send_Message->content, Send_Message->header.length);

            try
            {
                send(Send_Message->socket_fd, sendbuf, sendlen, 0);
            }
            catch (exception &e)
            {
                perror(e.what());
                LOGERROR("GateWay_Server::Send_Process , an unknown error :{}", e.what());
            }
            delete sendbuf;
        }
    }
}
void GateWay_Server::HeartBeatCheck_Process()
{
    while (!Process_stop)
    {
        for (auto map_it = HeartBeat_map.begin(); map_it != HeartBeat_map.end();)
        {
            (map_it->second)++;
            if ((map_it->second) > 6) // 5s*6没有收到心跳包，判定客户端掉线
            {

                LOGINFO("GateWay_Server::HeartBeatCheck_Process , fd {} timeout! disconnect", map_it->first);
                map_it = HeartBeat_map.erase(map_it);
                int fd = map_it->first;
                delfd(Epoll, fd);
                close(fd);

                Center_Server::Instance()->RemoveUser(fd);

                for (auto it = AllUser_list.begin(); it != AllUser_list.end(); it++)
                {
                    if (((*it)->Get_SocketFd()) == fd)
                    {
                        AllUser_list.erase(it);
                        break;
                    }
                    LOGINFO("GateWay_Server::HeartBeatCheck_Process timeout, user remove : {} , fd : {}", (*it)->Get_UserName(), (*it)->Get_SocketFd());
                }

                RateLimiter_Manager::Instance()->Pop(fd);
            }
            else
            {
                map_it++;
            }
        }
        this_thread::sleep_for(std::chrono::seconds(5)); // 定时五秒
    }
}

int GateWay_Server::OnProcess(Token_SocketMessage *Tokenmsg)
{
    const Header &header = Tokenmsg->msg->header;
    // const int socket_fd = Tokenmsg->msg->socket_fd;
    // char *content = Tokenmsg->msg->content;

    switch (header.type)
    {
    case ping_Request:
    {
        // OnPing(socket_fd, header, content);
        break;
    }

    // CenterServer
    case Request_Logout:
    case Request_CreateRoom:
    case Request_SerachRoom:
    case Request_JoinRoom:
    case Request_ExitRoom:
    case Request_KickRoom:
    case Request_SendRoomMessage:
    case Request_ChangeMap:
    case Request_Changelimit:
    case Request_ChangeReadyState:
    case Request_StartGame:
    case Request_ChangeTeam:
    case Request_SendPublicMessage:
    case Request_RoomInfo:
    {
        Center_Server::Instance()->OnProcess(Tokenmsg);
        break;
    }

    // LoginServer
    case Request_Login:
    case Request_Signup:
    case Request_Reconnect:
    {
        Login_Server::Instance()->OnProcess(Tokenmsg->msg);
        break;
    }
    }

    return 1;
}

void GateWay_Server::Push_NewUser(int socket_fd, sockaddr_in &tcp_addr)
{
    setnonblocking(socket_fd);

    User_Info *pre_user = GetUser(socket_fd);
    if (pre_user)
        RemoveUser(socket_fd);

    Socket_Info *info = new Socket_Info(socket_fd, tcp_addr);
    User_Info *user = new User_Info();
    user->sockinfo = info;
    user->states = USER_STATE::Logining;

    AllUser_list.emplace_back(user);

    int flag = 1;
    setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag));

    addfd(Epoll, socket_fd);
    RateLimiter_Manager::Instance()->Push(socket_fd);
    HeartBeat_map[socket_fd] = 0;
}

void GateWay_Server::RemoveUser(int fd)
{
    try
    {
        delfd(Epoll, fd);
        close(fd);

        // Login_Server::Instance()->RemoveFd(fd);
        Center_Server::Instance()->RemoveUser(fd);

        for (auto it = AllUser_list.begin(); it != AllUser_list.end(); it++)
        {
            if (((*it)->Get_SocketFd()) == fd)
            {
                if (*it)
                    delete (*it);
                AllUser_list.erase(it);
                break;
            }
            LOGINFO("GateWay_Server::RemoveUser , user remove : {}", (*it)->Get_UserName());
        }

        auto it = HeartBeat_map.find(fd);
        if (it != HeartBeat_map.end())
            HeartBeat_map.erase(it);

        RateLimiter_Manager::Instance()->Pop(fd);
    }
    catch (exception &e)
    {
        perror(e.what());
        LOGERROR("GateWay_Server::RemoveUser , an unknown error :{}", e.what());
    }
}

User_Info *GateWay_Server::GetUser(int fd)
{
    for (auto user : AllUser_list)
    {
        if (user->Get_SocketFd() == fd)
            return user;
    }
    return nullptr;
}