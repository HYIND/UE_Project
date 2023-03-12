#include "Login_Server.h"
#include "Hall_Server.h"

enum Login_Server_MsgNum
{
    Req_Login = 700,
    Req_Signup = 710,

    Response_Login = 705,
    Response_Signup = 715
};

template <typename T>
int Login_Server::Get_Header_Type(T &message)
{
    if (is_same<T, Login_Protobuf::Login_Response>::value)
        return Login_Server_MsgNum::Response_Login;
    else if (is_same<T, Login_Protobuf::Signup_Response>::value)
        return Login_Server_MsgNum::Response_Signup;
    else if (is_same<T, Login_Protobuf::Login_Request>::value)
        return Login_Server_MsgNum::Req_Login;
    else if (is_same<T, Login_Protobuf::Signup_Request>::value)
        return Login_Server_MsgNum::Req_Signup;
    return 0;
}
template <typename T>
void Login_Server::SendTo_SendQueue(int socket, T &message)
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

bool Login_Server::Init_Login()
{
    assert(socketpair(PF_UNIX, SOCK_DGRAM, 0, Login_pipe) != -1);
    setnonblocking(Login_pipe[1]);
    addfd(Login_epoll, Login_pipe[0]);

    return true;
}
int Login_Server::Run()
{
    Login_Process_stop = false;
    thread send_thread(&Login_Server::Send_Process, this);
    thread login_thread(&Login_Server::Login_Process, this);
    thread recv_thread(&Login_Server::Recv_Process, this);

    recv_thread.join();
    login_thread.join();
    send_thread.join();

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
    int stop = false;
    while (!stop)
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
            if ((Login_events[i].data.fd == Login_pipe[0]) && (Login_events[i].events & EPOLLIN))
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
                            stop = true;
                            break;
                        }
                        }
                    }
                }
            }
            else if (Login_events[i].events & EPOLLIN)
            {
                int socket_fd = Login_events[i].data.fd;
                int recv_length = 0;
                int re_num = recv(socket_fd, buffer, sizeof(Header), 0);
                while (re_num > 0)
                {
                    int count = 0;

                    recv_message = new Socket_Message(socket_fd);
                    // 获取头
                    recv_length += sizeof(Header);
                    memcpy(&recv_message->header, buffer, recv_length);

                    // 获取内容（可能为空）
                    if (recv_message->header.length > 0)
                    {
                        recv_message->content = new char[recv_message->header.length];
                        re_num = recv(socket_fd, recv_message->content, recv_message->header.length, 0);
                    }

                    RecvQueue.emplace(recv_message); // 投递消息
                    LoginProcess_cv.notify_one();    // 唤醒一个处理线程

                    if (count == 5) // 计数，防止持续占用
                        break;

                    recv_length = 0;
                    re_num = recv(socket_fd, buffer, sizeof(Header), 0);
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
            int ret = OnLoginProcess(Recv_Message);
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

int Login_Server::OnLoginProcess(Socket_Message *msg)
{
    const Header &header = msg->header;
    const int socket_fd = msg->socket_fd;
    const char *content = msg->content;

    switch (header.type)
    {
    case Req_Login:
    {
        OnLogin(socket_fd, header, content);
        break;
    }
    case Req_Signup:
    {
        OnSignup(socket_fd, header, content);
        break;
    }
    }
    delete msg;
    msg = nullptr;

    return 1;
}

void Login_Server::Push_Fd(int socket_fd, sockaddr_in &tcp_addr)
{
    setnonblocking(socket_fd);
    Socket_Info *info = new Socket_Info(socket_fd, tcp_addr);
    Login_fd_list.emplace_back(info);
    int flag = 1;
    setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(flag));
    addfd(Login_epoll, socket_fd);
}

void Login_Server::OnLogin(const int socket_fd, const Header header, const char *content)
{
    Login_Protobuf::Login_Request Request;
    Request.ParseFromArray(content, header.length);

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
            bool find_result = false;
            for (auto it = Login_fd_list.begin(); it != Login_fd_list.end(); it++)
            {
                Socket_Info *info = *it;
                if (info->tcp_fd == socket_fd)
                {
                    userinfo->SetSocketinfo_Move(info);
                    Login_fd_list.erase(it);
                    delfd(Login_epoll, socket_fd);
                    Hall_Server::Instance()->Push_User(userinfo);

                    Response.set_name(userinfo->Get_UserName());
                    Response.set_result((int)Login_Request_Result::LoginSuccess);
                    find_result = true;
                    break;
                }
            }
            if (!find_result)
                Response.set_result((int)Login_Request_Result::InnerError);
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
    if (!Mysql_Server::Instance()->Select("select* from Account where account = \'" +
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
    if (!Mysql_Server::Instance()->Update("INSERT INTO Account( name, account, password) VALUES(\'" +
                                              name + "\',\'" +
                                              acount + "\',\'" +
                                              password + "\') ",
                                          &affected_count))
        return Signup_Request_Result::RepeatError;

    if (affected_count > 0)
        return Signup_Request_Result::SignupSuccess;

    return Signup_Request_Result::InnerError;
}