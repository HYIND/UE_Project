#pragma once

#include "stdafx.h"

using namespace std;


class PipeServer
{

public:
	bool Init(int Port);
	void Listen();

public:
	HANDLE _Pipe;
	int _Port;
};


#define NumOfRecvThread 4
#define NumOfProcessThread 4
class Pipe_Manager
{
public:
	static Pipe_Manager* Instance()
	{
		static Pipe_Manager* m_Instance = new Pipe_Manager();
		return m_Instance;
	}

public:
	bool Init_PipeManager();
	void Add_Pipe(PipeServer* Pipe);
	void Del_Pipe(PipeServer* Pipe);
	int Run();
	void ThreadEnd();

public:
	template <typename T>
	void SendTo_DS(PipeServer* Pipe, T& message);

protected:
	void Pipe_Process();   // �����̺߳���

	int OnProcess(Socket_Message* msg);
	//void OnPing(const int socket_fd, const Header header, const char* content);

private:
	Pipe_Manager() {};

public:


private:
	// HIOCP
	HANDLE hIOCP;
	vector<HANDLE>Pipe_List;

	bool Process_stop = false;

private:
	// Logic Process
};

//hIOCP �ص��ṹ��
typedef struct PIPE_IO_DATA
{
	PipeServer* Pipe;
	OVERLAPPED ol;          //�ص��ṹ
	char buf[2048];			//���ݻ�����
	DWORD  dwResult;			//��/д���ֽ���
	int OperationType;     //I/O��������
#define OP_READ 1
#define OP_WRITE 2
}PIPE_IO_DATA, * PPIPE_IO_DATA;
