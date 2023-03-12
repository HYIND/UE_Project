#include "Hall_Server.h"

enum Hall_Server_MsgNum
{
    Req_Login = 700,
    Req_Signup = 710,

    Response_Login = 705,
    Response_Signout = 715
};

vector<int> game_pipe_list;

User_Info *Hall_Server::Get_Userinfo(int socket)
{
    for (auto v : user_list)
    {
        if (v->sockinfo->tcp_fd == socket)
            return v;
    }
    return nullptr;
}

template <typename T>
int Hall_Server::Get_Header_Type(T &message)
{
    // if (is_same<T, Login_Protobuf::Login_Response>::value)
    //     return Login_Server_MsgNum::Response_Login;
    // else if (is_same<T, Login_Protobuf::Signup_Response>::value)
    //     return Login_Server_MsgNum::Response_Signout;

    return 0;
}
template <typename T>
void Hall_Server::SendTo_SendQueue(int socket, T &message)
{
    Header header;
    header.type = Get_Header_Type(message);
    if (header.type == 0)
        return;
    header.length = message.ByteSizeLong();

    Socket_Message *msg = new Socket_Message(socket, header);
    msg->content = new char[header.length + 1];
    message.SerializeToArray(msg->content, header.length);

    SendQueue.emplace(msg);      // 投递消息
    SendProcess_cv.notify_one(); // 唤醒一个发送线程
}

bool Hall_Server::Init_Hall()
{
    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, Hall_pipe) != -1);
    setnonblocking(Hall_pipe[1]);
    addfd(Hall_epoll, Hall_pipe[0]);

    return true;
}
int Hall_Server::Run()
{
    Hall_Process_stop = false;

    thread send_thread(&Hall_Server::Send_Process, this);
    thread hall_threadpool[NumOfHallThread]; // 申请多个处理线程(线程池)，处理大厅信息
    for (int i = 0; i < NumOfHallThread; i++)
        hall_threadpool[i] = thread(&Hall_Server::Hall_Process, this);
    thread recv_thread(&Hall_Server::Recv_Process, this);

    recv_thread.join();
    for (int i = 0; i < NumOfHallThread; i++)
        hall_threadpool[i].join();
    send_thread.join();

    ThreadEnd();
    LOGINFO("Hall_Server::Run ,Hall_Server Close!");
    return 1;
}
void Hall_Server::ThreadEnd()
{
    close(Hall_pipe[0]);
    close(Hall_pipe[1]);
}
void Hall_Server::Recv_Process()
{
    char buffer[1024];
    memset(buffer, '\0', 1024);
    bool stop = false;
    Socket_Message *message = nullptr;
    while (!stop)
    {
        int num = epoll_wait(Hall_epoll, Hall_events, 200, -1);
        if (num < 0 && (errno != EINTR))
        {
            cout << "Hall_epoll failed!";
            break;
        }

        for (int i = 0; i < num; i++)
        {
            int socket = Hall_events[i].data.fd;
            if ((socket == Hall_pipe[0]) && (Hall_events[i].events & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(Hall_pipe[0], signals, 1023, 0);
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
            else if (Hall_events->events & EPOLLRDHUP)
            {
                delfd(Hall_epoll, socket);
                for (auto it = user_list.begin(); it != user_list.end(); it++)
                {
                    if ((*it)->Get_SocketFd() == socket)
                    {
                        user_list.erase(it);
                        break;
                    }
                }
                close(socket);
            }
            else if (Hall_events->events & EPOLLIN)
            {
                int recv_length = 0;
                int re_num = recv(socket, buffer, sizeof(Header), 0);
                while (re_num > 0)
                {
                    int count = 0;

                    message = new Socket_Message(socket);
                    // 获取头
                    recv_length += sizeof(Header);
                    memcpy(&message->header, buffer, recv_length);

                    // 获取内容（可能为空）
                    if (message->header.length > 0)
                    {
                        message->content = new char[message->header.length];
                        re_num = recv(socket, message->content, message->header.length, 0);
                    }

                    unique_lock<mutex> Queuelck(RecvQueue_mtx);
                    RecvQueue.emplace(message); // 投递消息
                    Queuelck.release()->unlock();
                    HallProcess_cv.notify_one(); // 唤醒其中一个处理线程

                    if (count == 5) // 计数，防止持续占用
                        break;

                    recv_length = 0;
                    re_num = recv(socket, buffer, sizeof(Header), 0);
                }
                if (re_num == 0)
                {
                    for (auto it = user_list.begin(); it != user_list.end(); it++)
                    {
                        if ((*it)->Get_SocketFd() == socket)
                        {
                            user_list.erase(it);
                            break;
                        }
                    }
                    close(socket);
                }
            }
        }
    }
    Hall_Process_stop = true;
    HallProcess_cv.notify_all();
    SendProcess_cv.notify_all();
    close(Hall_epoll);
}
void Hall_Server::Hall_Process()
{
    Socket_Message *Recv_Message = nullptr;
    while (!Hall_Process_stop)
    {
        unique_lock<mutex> Processlck(HallProcess_mtx);
        HallProcess_cv.wait(Processlck);
        Processlck.release()->unlock();

        while (!RecvQueue.empty())
        {
            // GetMessage
            unique_lock<mutex> Queuelck(RecvQueue_mtx);
            Recv_Message = RecvQueue.front();
            RecvQueue.pop();
            Queuelck.release()->unlock();

            if (!Recv_Message)
                continue;

            int ret = OnHallProcess(Recv_Message);
        }
    }
}
void Hall_Server::Send_Process()
{
    Socket_Message *Send_Message = nullptr;
    while (!Hall_Process_stop)
    {
        unique_lock<mutex> Sendlck(SendProcess_mtx);
        SendProcess_cv.wait(Sendlck);
        Sendlck.release()->unlock();

        while (!SendQueue.empty())
        {
            // GetMessage
            Send_Message = SendQueue.front();
            SendQueue.pop();

            if (!Send_Message)
                continue;

            // SendMessage
            int sendlen = sizeof(Header) + Send_Message->header.length;

            char *sendbuf = new char[sendlen];
            memset(sendbuf, '\0', sendlen);
            memcpy(sendbuf, &(Send_Message->header), sizeof(Header));
            memcpy(sendbuf + sizeof(Header), Send_Message->content, Send_Message->header.length);

            send(Send_Message->socket_fd, sendbuf, sendlen, 0);
            delete sendbuf;
        }
    }
}

int Hall_Server::OnHallProcess(Socket_Message *msg)
{
    const Header &header = msg->header;
    const int socket = msg->socket_fd;
    char *content = msg->content;

    switch (header.type)
    {
        // case 101:
        // {
        //     Get_hall_info(socket);
        //     break;
        // }
        // case 103:
        // {
        //     Hall_Message(socket, header, content);
        //     break;
        // }
        // case 104:
        // {
        //     Create_Room(socket);
        //     break;
        // }
        // case 105:
        // {
        //     Enter_Room(socket, header, content);
        //     break;
        // }
        // case 106:
        // {
        //     header.type = 206;
        //     char buf[sizeof(Header) + header.length] = {'\0'};
        //     memcpy(buf, &header, sizeof(Header));
        //     memcpy(buf + sizeof(Header), content, header.length);
        //     Hall_Protobuf::Ping_info P;
        //     P.ParseFromArray(content, header.length);
        //     int i = P.ping_id();
        //     send(socket, buf, sizeof(Header) + header.length, 0);
        //     break;
        // }
    }
    delete msg;
    msg = nullptr;

    return 1;
}

void Hall_Server::Push_User(User_Info *info)
{
    user_list.emplace_back(info);
    info->states = USER_STATE::Hall;
    addfd(Hall_epoll, info->Get_SocketFd());
}

void Hall_Server::Hall_Message(int socket, Header header, char *content)
{
    Hall_Protobuf::HallMessage_Request Req;
    Req.ParseFromArray(content, header.length);

    Hall_Protobuf::HallMessage_Response Res;

    Res.set_content(Req.content());
    Res.set_name(Get_Userinfo(socket)->Get_UserName());

    for (auto &v : user_list)
    {
        if (v->states == USER_STATE::Hall)
        {
            int send_sock = v->Get_SocketFd();
            if (send_sock == socket)
                continue;
            try
            {
                SendTo_SendQueue(send_sock, Res);
            }
            catch (exception &e)
            {
                continue;
            }
        }
    }
}

void Hall_Server::Get_hall_info(int sock_accept)
{
    Hall_Protobuf::HallInfo_Response Res;

    for (auto &v : user_list)
    {
        if (v->Get_SocketFd() == sock_accept || v->states != USER_STATE::Hall)
            continue;
        Hall_Protobuf::HallInfo_Response::User *info = Res.add_userinfo();
        info->set_name(v->name);
    }
    // for (auto &v : room_list)
    // {
    //     if (!(v->gaming))
    //     {
    //         Hall_Protobuf::HallInfo_Response::RoomInfo *info = Res.add_roominfo();
    //         info->set_room_id(v->room_id);
    //         info->set_host_name(Get_Userinfo(v->socket_host)->Get_UserName());
    //     }
    // }

    SendTo_SendQueue(sock_accept, Res);
}

void Hall_Server::Create_Room(int sock_accept)
{
    // Room_Process *newroom = new Room_Process(sock_accept);
    // room_counter++;
    // newroom->room_id = room_counter;
    // room_list.emplace_back(newroom);

    // thread T(&Room_Process::run, newroom);
    // T.detach();
    // newroom->Add_player(sock_accept);
}

void Hall_Server::Enter_Room(int sock_accept, Header header, char *content)
{
    Hall_Protobuf::EnterRoom_Request Req;
    Req.ParseFromArray(content, header.length);
    int id = Req.room_id();

    Hall_Protobuf::EnterRoom_Response Res;
    Res.set_room_id(id);
    // for (auto &v : room_list)
    // {
    //     if (v->room_id == id) // 房间可加入
    //     {
    //         if (v->user_count < v->user_limited)
    //         {
    //             Res.set_result(1);
    //             SendTo_SendQueue(sock_accept, Res);
    //             v->Add_player(sock_accept);
    //         }
    //         else // 房间人数已满
    //         {
    //             Res.set_result(0);
    //             SendTo_SendQueue(sock_accept, Res);
    //         }
    //     }
    //     else // 房间不存在/已解散
    //     {
    //         Res.set_result(-1);
    //         SendTo_SendQueue(sock_accept, Res);
    //     }
    // }
}
