#include "Center_Server.h"

// bool Center_Server::Init_CenterServer()
// {
//     assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, pipe) != -1);
//     setnonblocking(pipe[1]);
//     addfd(epoll, pipe[0]);
//     SigManager::Instance()->AddPipe(pipe[1]);

//     return true;
// }
// int Center_Server::Run()
// {
//     Process_stop = false;
//     thread heartcheck_thread(&Center_Server::HeartBeatCheck_Process, this);
//     thread send_thread(&Center_Server::Send_Process, this);
//     thread hall_threadpool[NumOfProcessThread]; // 申请多个处理线程(线程池)，处理大厅信息
//     for (int i = 0; i < NumOfProcessThread; i++)
//         hall_threadpool[i] = thread(&Center_Server::Center_Process, this);
//     thread recv_thread(&Center_Server::Recv_Process, this);

//     recv_thread.join();
//     for (int i = 0; i < NumOfProcessThread; i++)
//         hall_threadpool[i].join();
//     send_thread.join();
//     heartcheck_thread.join();

//     ThreadEnd();
//     LOGINFO("Center_Server::Run ,Center_Server Close!");
//     return 1;
// }
// void Center_Server::ThreadEnd()
// {
//     close(pipe[0]);
//     close(pipe[1]);
// }
// void Center_Server::Recv_Process()
// {
//     char buffer[1024];
//     memset(buffer, '\0', 1024);
//     while (!Process_stop)
//     {
//         int num = epoll_wait(epoll, events, 200, -1);
//         if (num < 0 && (errno != EINTR))
//         {
//             perror("Hall_epoll failed!");
//             break;
//         }

//         for (int i = 0; i < num; i++)
//         {
//             int socket_fd = ((Epoll_Data *)events[i].data.ptr)->fd;
//             uint32_t event = events[i].events;
//             if ((socket_fd == pipe[0]) && (event & EPOLLIN))
//             {
//                 int sig;
//                 char signals[1024];
//                 int ret = recv(pipe[0], signals, 1023, 0);
//                 if (ret == -1 || ret == 0)
//                     continue;
//                 else
//                 {
//                     for (i = 0; i < ret; i++)
//                     {
//                         switch (signals[i])
//                         {
//                         case SIGINT:
//                         case SIGTERM:
//                         {
//                             Process_stop = true;
//                             break;
//                         }
//                         }
//                     }
//                 }
//             }
//             else if (event & EPOLLRDHUP)
//             {
//                 MoveToWaitList(socket_fd);
//             }
//             else if (event & EPOLLIN)
//             {
//                 int re_num = recv(socket_fd, buffer, TokenSize + sizeof(Header), 0);
//                 while (re_num > 0)
//                 {
//                     int count = 0;
//                     Token_SocketMessage Token_Msg(socket_fd);

//                     // 获取token
//                     char token_ch[TokenSize];
//                     memcpy(token_ch, buffer, TokenSize);
//                     Token_Msg.token.assign(token_ch, TokenSize);

//                     // 获取头
//                     memcpy(&Token_Msg.msg->header, buffer + TokenSize, sizeof(Header));

//                     // 获取内容（可能为空）
//                     if (Token_Msg.msg->header.length > 0)
//                     {
//                         Token_Msg.msg->content = new char[Token_Msg.msg->header.length];
//                         re_num = recv(socket_fd, Token_Msg.msg->content, Token_Msg.msg->header.length, 0);
//                     }

//                     if (!CheckToken(Token_Msg.token))
//                     {
//                         ((Epoll_Data *)events[i].data.ptr)->wrongcount++;
//                         if (((Epoll_Data *)events[i].data.ptr)->wrongcount >= 5)
//                         {
//                             MoveToWaitList(socket_fd);
//                         }
//                         Token_Msg.Delete_SocketMessage();
//                         continue;
//                     }

//                     HeartBeat_map[socket_fd].count = 0;
//                     if (!RateLimiter_Manager::Instance()->TryPass(socket_fd) || Token_Msg.msg->header.type == Heart_Package)
//                     {
//                         Token_Msg.Delete_SocketMessage();
//                     }
//                     else
//                     {
//                         unique_lock<mutex> Queuelck(RecvQueue_mtx);
//                         RecvQueue.emplace(Token_Msg.msg); // 投递消息
//                         Queuelck.release()->unlock();
//                         HallProcess_cv.notify_one(); // 唤醒其中一个处理线程
//                     }

//                     if (++count == 5) // 计数，防止持续占用
//                         break;

//                     re_num = recv(socket_fd, buffer, TokenSize + sizeof(Header), 0);
//                 }
//                 // if (re_num <= 0)
//                 // {
//                 //     if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
//                 //     {
//                 //         // 处理错误
//                 //         MoveToWaitList(socket_fd);
//                 //     }
//                 // }
//             }
//         }
//     }
//     HallProcess_cv.notify_all();
//     SendProcess_cv.notify_all();
//     close(epoll);
// }
// void Center_Server::Center_Process()
// {
//     Socket_Message *Recv_Message = nullptr;
//     while (!Process_stop)
//     {
//         unique_lock<mutex> Processlck(HallProcess_mtx);
//         HallProcess_cv.wait(Processlck);
//         Processlck.release()->unlock();

//         while (!RecvQueue.empty())
//         {
//             // GetMessage
//             RecvQueue_mtx.lock();
//             Recv_Message = RecvQueue.front();
//             RecvQueue.pop();
//             RecvQueue_mtx.unlock();

//             if (!Recv_Message)
//                 continue;

//             int ret = OnProcess(Recv_Message);
//         }
//     }
// }
// void Center_Server::Send_Process()
// {
//     Socket_Message *Send_Message = nullptr;
//     while (!Process_stop)
//     {
//         unique_lock<mutex> Sendlck(SendProcess_mtx);
//         SendProcess_cv.wait(Sendlck);
//         Sendlck.release()->unlock();

//         while (!SendQueue.empty())
//         {
//             // GetMessage
//             Send_Message = SendQueue.front();
//             SendQueue.pop();

//             if (!Send_Message)
//                 continue;

//             // SendMessage
//             int sendlen = sizeof(Header) + Send_Message->header.length;

//             char *sendbuf = new char[sendlen];
//             memset(sendbuf, '\0', sendlen);
//             memcpy(sendbuf, &(Send_Message->header), sizeof(Header));
//             memcpy(sendbuf + sizeof(Header), Send_Message->content, Send_Message->header.length);

//             send(Send_Message->socket_fd, sendbuf, sendlen, 0);
//             delete sendbuf;
//         }
//     }
// }
// void Center_Server::HeartBeatCheck_Process()
// {
//     while (!Process_stop)
//     {
//         for (auto map_it = HeartBeat_map.begin(); map_it != HeartBeat_map.end(); map_it++)
//         {
//             (map_it->second.count)++;
//             if ((map_it->second.count) > 6) // 5s*6没有收到心跳包，判定客户端掉线
//             {
//                 LOGINFO("Center_Server::HeartBeatCheck_Process , fd {} timeout! disconnect", map_it->first);
//                 int fd = map_it->first;
//                 try
//                 {
//                     delfd(epoll, fd);
//                     close(fd);
//                     for (auto it = user_list.begin(); it != user_list.end(); it++)
//                     {
//                         if ((*it)->Get_SocketFd() == fd)
//                         {
//                             User_Info *user = (*it);
//                             user_list.erase(it);
//                             wait_list.emplace_back(user);
//                             LOGINFO("Center_Server::HeartBeatCheck_Process ,user timeout waiting for reconnect: {}", user->Get_UserName());
//                             break;
//                         }
//                     }
//                     map_it = HeartBeat_map.erase(map_it);
//                 }
//                 catch (exception &e)
//                 {
//                     perror(e.what());
//                     LOGINFO("Center_Server::MoveToWaitList , an unknown error :{}", e.what());
//                 }
//             }
//         }
//         for (auto it = wait_list.begin(); it != wait_list.end();)
//         {
//             if (++(it->count) >= 6) // 5s*6未重连，移出等待列表
//             {
//                 if (it->userdata)
//                     delete (it->userdata);
//                 it = wait_list.erase(it);
//             }
//             else if (it->count < 6 && it->count >= 0)
//             {
//                 (it->count)++;
//                 it++;
//             }
//         }
//         this_thread::sleep_for(std::chrono::seconds(5)); // 定时五秒
//     }
// }

User_Info *Center_Server::Get_Userinfo(int socket_fd)
{
    for (auto v : user_list)
    {
        if (v->sockinfo->tcp_fd == socket_fd)
            return v;
    }
    return nullptr;
}

int Center_Server::OnProcess(Socket_Message *msg)
{
    const Header &header = msg->header;
    const int socket_fd = msg->socket_fd;
    char *content = msg->content;

    switch (header.type)
    {
    case Request_Logout:
    {
        OnLogout(socket_fd, header, content);
    }
    case ping_Request:
    {
        // OnPing(socket_fd, header, content);
        break;
    }

    // Room
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
    {
        Room_Manager::Instance()->Process(msg);
        break;
    }

    // Hall
    case Request_SendPublicMessage:
    {
        Hall_Manager::Instance()->Process(msg);
        break;
    }
    }
    delete msg;
    msg = nullptr;

    return 1;
}

int Center_Server::OnProcess(Token_SocketMessage *TokenMsg)
{
    if (!CheckToken(TokenMsg->token))
        return 0;
    Socket_Message *msg = TokenMsg->msg;
    const Header &header = msg->header;
    const int socket_fd = msg->socket_fd;
    char *content = msg->content;

    bool result = false;
    switch (header.type)
    {
    case Request_Logout:
    {
        OnLogout(socket_fd, header, content);
        result = true;
        break;
    }
    case ping_Request:
    {
        // OnPing(socket_fd, header, content);
        result = true;
        break;
    }

    // Room
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
    case Request_RoomInfo:
    {
        Room_Manager::Instance()->Process(msg);
        result = true;
        break;
    }

    // Hall
    case Request_SendPublicMessage:
    {
        Hall_Manager::Instance()->Process(msg);
        result = true;
        break;
    }
    }

    return result ? 1 : 0;
}

// void Center_Server::Push_User(User_Info *userinfo)
// {
//     userinfo->states = USER_STATE::Hall;

//     user_list.emplace_back(userinfo);
//     addfd(epoll, userinfo->Get_SocketFd());
//     HeartCount heartcount(userinfo);
//     HeartBeat_map.insert(pair<int, HeartCount>(userinfo->Get_SocketFd(), heartcount));
//     Token_map[userinfo->Get_Token()] = userinfo;

//     LOGINFO("Center_Server::Push_User user login : {} IP : {} Token : {}", userinfo->Get_UserName(), userinfo->Get_IP(), userinfo->token);
// }

void Center_Server::Push_LoginUser(User_Info *userinfo)
{
    userinfo->states = USER_STATE::Hall;

    user_list.emplace_back(userinfo);
    Token_map[userinfo->Get_Token()] = userinfo;

    LOGINFO("Center_Server::Push_LoginUser user login : {} IP : {} Token : {}", userinfo->Get_UserName(), userinfo->Get_IP(), userinfo->token);
}

// void Center_Server::OnLogout(const int socket_fd, const Header header, const char *content)
// {
//     User_Info *userinfo = Get_Userinfo(socket_fd);
//     if (userinfo)
//     {
//         delfd(epoll, socket_fd);
//         Login_Server::Instance()->Push_Fd(*(userinfo->sockinfo));
//         for (auto it = user_list.begin(); it != user_list.end(); it++)
//         {
//             if ((*it) == userinfo)
//             {
//                 LOGINFO("Center_Server::OnLogout user logout : {}", userinfo->Get_UserName());
//                 if (*it)
//                     delete (*it);
//                 user_list.erase(it);
//                 break;
//             }
//         }
//         auto it = HeartBeat_map.find(socket_fd);
//         if (it != HeartBeat_map.end())
//             HeartBeat_map.erase(it);
//     }
// }

void Center_Server::OnLogout(const int socket_fd, const Header header, const char *content)
{
    User_Info *user = Get_Userinfo(socket_fd);
    if (!user)
        return;

    auto it = Token_map.find(user->Get_Token());
    if (it != Token_map.end())
        Token_map.erase(it);
    user->states = USER_STATE::Logining;

    // delfd(epoll, socket_fd);
    for (auto it = user_list.begin(); it != user_list.end(); it++)
    {
        if ((*it) == user)
        {
            LOGINFO("Center_Server::OnLogout user logout : {}", user->Get_UserName());
            user_list.erase(it);
            break;
        }
    }
}

// void Center_Server::OnPing(const int socket_fd, const Header header, const char *content)
// {
//     Hall_Protobuf::Ping_Request Request;
//     Request.ParseFromArray(content, header.length);

//     Hall_Protobuf::Ping_Response Response;
//     Response.set_pingpack_id(Request.pingpack_id());

//     SendTo_SendQueue(socket_fd, Response);
// }

// bool Center_Server::Check_Reconnect(Socket_Info *sockinfo, string &token)
// {
//     Login_Protobuf::Reconnect_Response Response;

//     auto map_it = Token_map.find(token);
//     if (map_it == Token_map.end())
//     {
//         Response.set_result((int)Reconnect_Result::Fail);
//         SendTo_SendQueue(sockinfo->Get_SocketFd(), Response);
//         return false;
//     }
//     Token_map.erase(map_it);

//     for (auto it = wait_list.begin(); it != wait_list.end(); it++)
//     {
//         HeartCount &waituser = *it;
//         if (waituser.userdata->Get_Token() == token)
//         {
//             wait_list.erase(it);

//             User_Info *user = waituser.userdata;
//             user->SetSocketinfo_Move(sockinfo);
//             user->states = USER_STATE::Hall;
//             user->token = Get_Token();

//             user_list.emplace_back(user);
//             addfd(epoll, sockinfo->Get_SocketFd());
//             HeartCount heartcount(user);
//             HeartBeat_map.insert(pair<int, HeartCount>(user->Get_SocketFd(), heartcount));
//             Token_map[user->Get_Token()] = user;

//             Response.set_result((int)Reconnect_Result::Success);
//             Response.set_token(user->token);

//             SendTo_SendQueue(user->Get_SocketFd(), Response);

//             return true;
//         }
//     }
//     Response.set_result((int)Reconnect_Result::Fail);
//     SendTo_SendQueue(sockinfo->Get_SocketFd(), Response);
//     return false;
// }

bool Center_Server::CheckToken(string &token)
{
    auto map_it = Token_map.find(token);
    if (map_it == Token_map.end())
    {
        LOGERROR("Center_Server::CheckToken ErrorToken : {} ", token);
        return false;
    }
    return true;
}

void Center_Server::RemoveUser(int fd)
{
    try
    {
        Room_Manager::Instance()->RemoveUser(fd);
        for (auto it = user_list.begin(); it != user_list.end(); it++)
        {
            if (((*it)->Get_SocketFd()) == fd)
            {
                user_list.erase(it);
                break;
            }
            LOGINFO("Center_Server::RemoveUser , user remove : {}", (*it)->Get_UserName());
        }
        for (auto it = Token_map.begin(); it != Token_map.end(); it++)
        {
            if (((*it).second->Get_SocketFd()) == fd)
            {
                Token_map.erase(it);
                break;
            }
        }
        // auto it = HeartBeat_map.find(fd);
        // if (it != HeartBeat_map.end())
        //     HeartBeat_map.erase(it);
    }
    catch (exception &e)
    {
        perror(e.what());
        LOGINFO("Center_Server::RemoveUser , an unknown error :{}", e.what());
    }
}

// void Center_Server::MoveToWaitList(int fd)
// {
//     try
//     {
//         delfd(epoll, fd);
//         close(fd);
//         for (auto it = user_list.begin(); it != user_list.end(); it++)
//         {
//             if ((*it)->Get_SocketFd() == fd)
//             {
//                 User_Info *user = (*it);
//                 user->states = USER_STATE::Disconnection;
//                 user_list.erase(it);
//                 wait_list.emplace_back(user);
//                 LOGINFO("Center_Server::MoveToWaitList ,user timeout waiting for reconnect: {}", user->Get_UserName());
//                 break;
//             }
//         }
//         auto it = HeartBeat_map.find(fd);
//         if (it != HeartBeat_map.end())
//             HeartBeat_map.erase(it);
//     }
//     catch (exception &e)
//     {
//         perror(e.what());
//         LOGINFO("Center_Server::MoveToWaitList , an unknown error :{}", e.what());
//     }
// }
