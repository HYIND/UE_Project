#pragma once

#pragma once

#include "stdafx.h"
#include "NetworkBase.h"

#include "MsgNum.h"

using namespace std;

SOCKET Get_NewSocket(std::string IP, uint16_t socket_port, int protocol, sockaddr_in& addr);

#define NumOfRecvThread 4
#define NumOfProcessThread 4
class Net_Server
{
public:
	static Net_Server* Instance()
	{
		static Net_Server* m_Instance = new Net_Server();
		return m_Instance;
	}

public:
	bool Init_NetServer();
	int Run();
	void ThreadEnd();

public:
	template <typename T>
	void SendTo_SendQueue(int socket, T& message);

protected:
	void Recv_Process();   // 接收线程函数
	void Net_Process(); // 处理线程函数
	void Send_Process();   // 发送线程函数

	int OnProcess(Socket_Message* msg);
	void OnPing(const int socket_fd, const Header header, const char* content);

private:
	Net_Server() {};

public:


private:
	// HIOCP
	HANDLE hIOCP;
	SOCKET Socket = INVALID_SOCKET;
	sockaddr_in Socket_RemoteAddr;

	// Process
	queue<Socket_Message*> RecvQueue; // 接收队列
	queue<Socket_Message*> SendQueue; // 发送队列

	mutex NetProcess_mtx;             // 唤醒处理线程的锁
	condition_variable NetProcess_cv; // 唤醒处理线程的条件变量
	mutex SendProcess_mtx;             // 唤醒发送线程的锁
	condition_variable SendProcess_cv; // 唤醒发送线程的条件变量

	mutex RecvQueue_mtx; // 接收队列的锁
	mutex SendQueue_mtx; // 发送队列的锁

	bool Process_stop = false;

private:
	// Logic Process
};
