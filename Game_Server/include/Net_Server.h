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
	void Recv_Process();   // �����̺߳���
	void Net_Process(); // �����̺߳���
	void Send_Process();   // �����̺߳���

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
	queue<Socket_Message*> RecvQueue; // ���ն���
	queue<Socket_Message*> SendQueue; // ���Ͷ���

	mutex NetProcess_mtx;             // ���Ѵ����̵߳���
	condition_variable NetProcess_cv; // ���Ѵ����̵߳���������
	mutex SendProcess_mtx;             // ���ѷ����̵߳���
	condition_variable SendProcess_cv; // ���ѷ����̵߳���������

	mutex RecvQueue_mtx; // ���ն��е���
	mutex SendQueue_mtx; // ���Ͷ��е���

	bool Process_stop = false;

private:
	// Logic Process
};
