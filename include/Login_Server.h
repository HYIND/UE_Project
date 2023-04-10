#pragma once

#include "header.h"

using namespace std;

enum class Login_Request_Result
{
    InnerError = -1,  // 内部错误
    AccountError = 0, // 账号密码错误
    LoginSuccess = 1  // 登录成功
};

enum class Signup_Request_Result
{
    InnerError = -1,  // 内部错误
    RepeatError = 0,  // 名称/账号/密码 重复
    SignupSuccess = 1 // 注册成功
};

enum class Reconnect_Result
{
	Fail = 0,
	Success = 1
};

class Login_Server
{
    /* 以下为基础服务框架 */
public:
    static Login_Server *Instance()
    {
        static Login_Server *m_Instance = new Login_Server();
        return m_Instance;
    }

private:
    Login_Server(){};

public:
    bool Init_Login();
    int Run();
    void ThreadEnd();

    int Get_pipe() { return Login_pipe[1]; };

protected:
    void Recv_Process();           // 接收线程函数
    void Login_Process();          // 处理线程函数
    void Send_Process();           // 发送线程函数
    void HeartBeatCheck_Process(); // 心跳检测函数

    template <typename T>
    int Get_Header_Type(T &message);
    template <typename T>
    void SendTo_SendQueue(int socket, T &message);

private:
    // Epoll
    int Login_pipe[2];
    int Login_epoll = epoll_create(100);
    epoll_event Login_events[100];

    // Process
    queue<Socket_Message *> RecvQueue; // 接收队列
    queue<Socket_Message *> SendQueue; // 发送队列

    mutex LoginProcess_mtx;             // 唤醒处理线程的锁
    condition_variable LoginProcess_cv; // 唤醒处理线程的条件变量
    mutex SendProcess_mtx;              // 唤醒发送线程的锁
    condition_variable SendProcess_cv;  // 唤醒发送线程的条件变量
    bool Login_Process_stop = false;

    /* 以下为逻辑处理 */
public:
    void Push_Fd(int socket_fd, sockaddr_in &tcp_addr);
    void Push_Fd(Socket_Info &info);

protected:
    int OnLoginProcess(Socket_Message *msg);
    void OnLogin(const int socket_fd, const Header header, const char *content);
    void OnSignup(const int socket_fd, const Header header, const char *content);
    void OnReconnect(const int socket_fd, const Header header, const char *content);

    Login_Request_Result Check_Login(string &acount, string &password, User_Info **userinfo);
    Signup_Request_Result Check_Signup(string &name, string &acount, string &password);

    void RemoveFd(int fd);

private:
    // Logic Process
    vector<Socket_Info *> Login_fd_list;
    map<int, int> HeartBeat_map; //<fd->count>
};