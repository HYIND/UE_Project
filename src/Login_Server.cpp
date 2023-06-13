#include "Login_Server.h"
#include "GateWay_Server.h"
#include "Center_Server.h"
#include "RateLimiter.h"
#include "MsgNum.h"

template <typename T>
int Login_Server::Get_Header_Type(T &message)
{
    if (is_same<T, Login_Protobuf::Login_Response>::value)
        return Login_Server_MsgNum::Response_Login;
    else if (is_same<T, Login_Protobuf::Signup_Response>::value)
        return Login_Server_MsgNum::Response_Signup;
    else if (is_same<T, Login_Protobuf::Login_Request>::value)
        return Login_Server_MsgNum::Request_Login;
    else if (is_same<T, Login_Protobuf::Signup_Request>::value)
        return Login_Server_MsgNum::Request_Signup;
    return 0;
}
template <typename T>
void Login_Server::SendTo_SendQueue(int socket, T &message)
{
    // Header header;
    // header.type = Get_Header_Type(message);
    // if (header.type == 0)
    //     return;
    // header.length = message.ByteSizeLong();

    // Socket_Message *msg = new Socket_Message(socket, header);
    // msg->content = new char[header.length + 1];
    // message.SerializeToArray(msg->content, header.length);

    // SendQueue.emplace(msg);      // 投递消息
    // SendProcess_cv.notify_one(); // 唤醒一个发送线程

    GateWay_Server::Instance()->SendTo_SendQueue(socket, message);
}

bool Login_Server::Init_Login()
{
    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, Login_pipe) != -1);
    setnonblocking(Login_pipe[1]);
    addfd(Login_epoll, Login_pipe[0]);
    SigManager::Instance()->AddPipe(Login_pipe[1]);

    return true;
}
int Login_Server::Run()
{
    Login_Process_stop = false;
    thread heartcheck_thread(&Login_Server::HeartBeatCheck_Process, this);
    thread send_thread(&Login_Server::Send_Process, this);
    thread login_thread(&Login_Server::Login_Process, this);
    thread recv_thread(&Login_Server::Recv_Process, this);

    recv_thread.join();
    login_thread.join();
    send_thread.join();
    heartcheck_thread.join();

    ThreadEnd();
    LOGINFO("Login_Server::Run ,Login_Server Close!");
    return 1;
}
void Login_Server::ThreadEnd()
{
    close(Login_pipe[0]);
    close(Login_pipe[1]);
}
void Login_Server::Recv_Process()
{
    // int timefd = timerfd_create(CLOCK_MONOTONIC, 0);
    // itimerspec timer;

    // timeval now;
    // gettimeofday(&now, NULL);
    // timer.it_value.tv_sec =10;
    // timer.it_value.tv_nsec = 0;
    // timer.it_interval.tv_sec = 5;
    // timer.it_interval.tv_nsec = 0;
    // timerfd_settime(timefd, TFD_TIMER_ABSTIME, &timer, NULL);
    // addfd(Login_epoll, timefd, true);

    char buffer[1024];
    memset(buffer, '\0', 1024);
    Socket_Message *recv_message = nullptr;
    while (!Login_Process_stop)
    {
        int number = epoll_wait(Login_epoll, Login_events, 200, -1);
        if (number < 0 && (errno != EINTR))
        {
            cout << "Login_epoll failure\n";
            break;
        }
        for (int i = 0; i < number; i++)
        {
            // if ((Login_events[i].data.fd == timefd) && (Login_events[i].events & EPOLLIN))
            // {
            //     cout << "timer out!\n";
            //     uint64_t exp = 0;
            //     int ret=read(timefd, &exp, sizeof(uint64_t));
            //     cout<<exp;
            //     // addfd(Login_epoll,timefd,false);
            // }
            int socket_fd = ((Epoll_Data *)Login_events[i].data.ptr)->fd;
            uint32_t event = Login_events[i].events;
            if ((socket_fd == Login_pipe[0]) && (event & EPOLLIN))
            {
                int sig;
                char signals[1024];
                int ret = recv(Login_pipe[0], signals, 1023, 0);
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
                            Login_Process_stop = true;
                            break;
                        }
                        }
                    }
                }
            }
            else if (event & EPOLLRDHUP)
            {
                RemoveFd(socket_fd);
            }
            else if (event & EPOLLIN)
            {
                int re_num = recv(socket_fd, buffer, TokenSize + sizeof(Header), 0);
                while (re_num > 0)
                {
                    int count = 0;

                    recv_message = new Socket_Message(socket_fd);

                    // 获取token
                    char token_ch[TokenSize];
                    memcpy(token_ch, buffer, TokenSize);

                    // 获取头
                    memcpy(&recv_message->header, buffer + TokenSize, sizeof(Header));

                    // 获取内容（可能为空）
                    if (recv_message->header.length > 0)
                    {
                        recv_message->content = new char[recv_message->header.length];
                        re_num = recv(socket_fd, recv_message->content, recv_message->header.length, 0);
                    }

                    HeartBeat_map[socket_fd] = 0;
                    if (!RateLimiter_Manager::Instance()->TryPass(socket_fd) || recv_message->header.type == Heart_Package)
                    {
                        delete (recv_message);
                    }
                    else
                    {
                        RecvQueue.emplace(recv_message); // 投递消息
                        LoginProcess_cv.notify_one();    // 唤醒一个处理线程
                    }

                    if (++count == 5) // 计数，防止持续占用
                        break;

                    re_num = recv(socket_fd, buffer, TokenSize + sizeof(Header), 0);
                }
            }
        }
    }
    Login_Process_stop = true;
    LoginProcess_cv.notify_all();
    SendProcess_cv.notify_all();
    // close(timefd);
    close(Login_epoll);
}
void Login_Server::Login_Process()
{
    Socket_Message *Recv_Message = nullptr;
    while (!Login_Process_stop)
    {
        unique_lock<mutex> Processlck(LoginProcess_mtx);
        LoginProcess_cv.wait(Processlck);
        Processlck.release()->unlock();

        while (!RecvQueue.empty())
        {
            // GetMessage
            Recv_Message = RecvQueue.front();
            RecvQueue.pop();

            if (!Recv_Message)
                continue;

            // Process
            int ret = OnProcess(Recv_Message);
        }
    }
}
void Login_Server::Send_Process()
{
    Socket_Message *Send_Message = nullptr;
    while (!Login_Process_stop)
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
void Login_Server::HeartBeatCheck_Process()
{
    while (!Login_Process_stop)
    {
        for (auto map_it = HeartBeat_map.begin(); map_it != HeartBeat_map.end(); map_it++)
        {
            (map_it->second)++;
            if ((map_it->second) > 6) // 5s*6没有收到心跳包，判定客户端掉线
            {
                LOGINFO("Login_Server::HeartBeatCheck_Process , fd {} timeout! disconnect", map_it->first);
                int fd = map_it->first;
                RemoveFd(fd);
            }
        }
        this_thread::sleep_for(std::chrono::seconds(5)); // 定时五秒
    }
}

// int Login_Server::OnProcess(Socket_Message *msg)
// {
//     const Header &header = msg->header;
//     const int socket_fd = msg->socket_fd;
//     const char *content = msg->content;

//     switch (header.type)
//     {
//     case Request_Login:
//     {
//         OnLogin(socket_fd, header, content);
//         break;
//     }
//     case Request_Signup:
//     {
//         OnSignup(socket_fd, header, content);
//         break;
//     }
//     case Request_Reconnect:
//     {
//         OnReconnect(socket_fd, header, content);
//         break;
//     }
//     }
//     delete msg;
//     msg = nullptr;

//     return 1;
// }

int Login_Server::OnProcess(Socket_Message *msg)
{
    const Header &header = msg->header;
    const int socket_fd = msg->socket_fd;
    const char *content = msg->content;

    bool result = false;
    switch (header.type)
    {
    case Request_Login:
    {
        OnLogin(socket_fd, header, content);
        result = true;
        break;
    }
    case Request_Signup:
    {
        OnSignup(socket_fd, header, content);
        result = true;
        break;
    }
    case Request_Reconnect:
    {
        OnReconnect(socket_fd, header, content);
        result = true;
        break;
    }
    }

    return result ? 1 : 0;
}

void Login_Server::Push_Fd(int socket_fd, sockaddr_in &tcp_addr)
{
    setnonblocking(socket_fd);
    Socket_Info *info = new Socket_Info(socket_fd, tcp_addr);
    Login_fd_list.emplace_back(info);
    int flag = 1;
    setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag));
    addfd(Login_epoll, socket_fd);
    RateLimiter_Manager::Instance()->Push(socket_fd);
    HeartBeat_map[socket_fd] = 0;
}

void Login_Server::Push_Fd(Socket_Info &info_in)
{
    setnonblocking(info_in.Get_SocketFd());
    Socket_Info *info = new Socket_Info(info_in);
    Login_fd_list.emplace_back(info);
    int flag = 1;
    setsockopt(info_in.Get_SocketFd(), IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag));
    addfd(Login_epoll, info_in.Get_SocketFd());
    HeartBeat_map[info_in.Get_SocketFd()] = 0;
    RateLimiter_Manager::Instance()->Push(info_in.Get_SocketFd());
}

void Login_Server::OnLogin(const int socket_fd, const Header header, const char *content)
{
    Login_Protobuf::Login_Request Request;
    Request.ParseFromArray(content, header.length);

    User_Info *user = GateWay_Server::Instance()->GetUser(socket_fd);
    if (!user || user->states != USER_STATE::Logining)
        return;

    string account = Request.logininfo().account();
    string password = Request.logininfo().password();
    User_Info *userinfo = nullptr;

    Login_Protobuf::Login_Response Response;
    Response.set_result((int)Check_Login(account, password, &userinfo));

    if (Response.result() == (int)Login_Request_Result::LoginSuccess)
    {
        if (!userinfo)
            Response.set_result((int)Login_Request_Result::InnerError);
        else
        {
            user->id = userinfo->Get_ID();
            user->name = userinfo->Get_UserName();

            string token;
            bool result = false;
            do
            {
                token = Get_Token();
                result = Center_Server::Instance()->CheckToken(token);
            } while (result);

            user->token = token;
            Center_Server::Instance()->Push_LoginUser(user);

            Response.set_name(user->Get_UserName());
            Response.set_result((int)Login_Request_Result::LoginSuccess);
            Response.set_token(user->Get_Token());
        }
    }
    SendTo_SendQueue(socket_fd, Response);
}

void Login_Server::OnSignup(const int socket_fd, const Header header, const char *content)
{
    Login_Protobuf::Signup_Request Request;
    Request.ParseFromArray(content, header.length);

    string name = Request.name();
    string account = Request.signupinfo().account();
    string password = Request.signupinfo().password();

    Login_Protobuf::Signup_Response Response;
    Response.set_result((int)Check_Signup(name, account, password));

    SendTo_SendQueue(socket_fd, Response);
}

Login_Request_Result Login_Server::Check_Login(string &acount, string &password, User_Info **userinfo)
{
    MYSQL_RES *Res;
    int count = -1;
    if (!Mysql_Server::Instance()->Query("select* from Account where account = \'" +
                                             acount +
                                             "\' and password = \'" +
                                             password +
                                             "\'",
                                         &Res, &count))
        return Login_Request_Result::InnerError;

    if (count > 0 && Res)
    {
        MYSQL_ROW row = mysql_fetch_row(Res);
        if (userinfo)
        {
            *userinfo = new User_Info(atoi(row[0]), string(row[1]));
        }
        return Login_Request_Result::LoginSuccess;
    }
    return Login_Request_Result::AccountError;
}

Signup_Request_Result Login_Server::Check_Signup(string &name, string &acount, string &password)
{

    int affected_count = -1;
    // if (!Mysql_Server::Instance()->Update("INSERT INTO Account( name, account, password) VALUES(\'" +
    //                                           name + "\',\'" +
    //                                           acount + "\',\'" +
    //                                           password + "\') ",
    //                                       &affected_count))
    //     return Signup_Request_Result::RepeatError;

    time_t now = time(0);
    tm *ltm = localtime(&now);
    //    to_string(1900 + ltm->tm_year) + "-" + to_string(ltm->tm_mon) + "-" + to_string(1900 + ltm->tm_year)
    //     + " " + inttostring(t.GetHour()) + ":" + inttostring(t.GetMinute()) + ":" + inttostring(t.GetSecond());

    string strtime = fmt::format("{}-{}-{} {}:{}:{}",
                                 to_string(1900 + ltm->tm_year), to_string(ltm->tm_mon), to_string(ltm->tm_mday),
                                 to_string(ltm->tm_hour), to_string(ltm->tm_min), to_string(ltm->tm_sec));

    if (!Mysql_Server::Instance()->Update("INSERT INTO Account( name, account, password, createtime) VALUES(\'" +
                                              name + "\',\'" +
                                              acount + "\',\'" +
                                              password + "\',\'" +
                                              strtime + "\') ",
                                          &affected_count))
        return Signup_Request_Result::RepeatError;

    if (affected_count > 0)
        return Signup_Request_Result::SignupSuccess;

    return Signup_Request_Result::InnerError;
}

void Login_Server::RemoveFd(int fd)
{
    try
    {
        delfd(Login_epoll, fd);
        close(fd);
        for (auto it = Login_fd_list.begin(); it != Login_fd_list.end(); it++)
        {
            if ((*it)->Get_SocketFd() == fd)
            {
                Login_fd_list.erase(it);
                if (*it)
                    delete (*it);
                break;
            }
        }
        auto it = HeartBeat_map.find(fd);
        if (it != HeartBeat_map.end())
            HeartBeat_map.erase(it);
        RateLimiter_Manager::Instance()->Pop(fd);
    }
    catch (exception &e)
    {
        perror(e.what());
        LOGINFO("Login_Server::RemoveFd , an unknown error :{}", e.what());
    }
}

void Login_Server::OnReconnect(const int socket_fd, const Header header, const char *content)
{
    // Login_Protobuf::Reconnect_Request Request;
    // Request.ParseFromArray(content, header.length);
    // string Token = Request.token();

    // auto map_it = HeartBeat_map.find(socket_fd);
    // if (map_it != HeartBeat_map.end())
    //     HeartBeat_map.erase(map_it);

    // bool find_result = false;
    // for (auto it = Login_fd_list.begin(); it != Login_fd_list.end(); it++)
    // {
    //     Socket_Info *info = *it;
    //     if (info->tcp_fd == socket_fd)
    //     {
    //         Login_fd_list.erase(it);
    //         delfd(Login_epoll, socket_fd);
    //         HeartBeat_map.erase(socket_fd);
    //         if (!Center_Server::Instance()->Check_Reconnect(info, Token))
    //         {
    //             Login_fd_list.emplace_back(info);
    //             addfd(Login_epoll, socket_fd);
    //             HeartBeat_map[socket_fd] = 0;
    //             return;
    //         }
    //         find_result = true;
    //         return;
    //     }
    // }
    // HeartBeat_map[socket_fd] = 0;
}

void Login_Server::OnLogin(Login_Protobuf::Login_Request &Request, Login_Protobuf::Login_Response &Response)
{
    // User_Info *user = GateWay_Server::Instance()->GetUser(socket_fd);
    // if (!user || user->states != USER_STATE::Logining)
    //     return;

    // string account = Request.logininfo().account();
    // string password = Request.logininfo().password();
    // User_Info *userinfo = nullptr;

    // Response.set_result((int)Check_Login(account, password, &userinfo));

    // if (Response.result() == (int)Login_Request_Result::LoginSuccess)
    // {
    //     if (!userinfo)
    //         Response.set_result((int)Login_Request_Result::InnerError);
    //     else
    //     {
    //         user->id = userinfo->Get_ID();
    //         user->name = userinfo->Get_UserName();

    //         string token;
    //         bool result = false;
    //         do
    //         {
    //             token = Get_Token();
    //             result = Center_Server::Instance()->CheckToken(token);
    //         } while (result);

    //         user->token = token;
    //         Center_Server::Instance()->Push_LoginUser(user);

    //         Response.set_name(user->Get_UserName());
    //         Response.set_result((int)Login_Request_Result::LoginSuccess);
    //         Response.set_token(user->Get_Token());
    //     }
    // }
}