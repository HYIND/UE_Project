#include "PipeServer.h"
#include "Game_Manager.h"

bool PipeServer::Init(int Port)
{
	_Port = Port;

	// Create named pipe
	_Pipe = CreateNamedPipe(
		L"\\\\.\\pipe\\MyPipe", // pipe name
		PIPE_ACCESS_DUPLEX, // read/write access
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, // message type pipe, message-read mode, blocking mode
		PIPE_UNLIMITED_INSTANCES, // max. instances
		1024, // output buffer size
		1024, // input buffer size
		0, // client time-out
		NULL // default security attribute
	);

	if (_Pipe == INVALID_HANDLE_VALUE)
	{
		LOGINFO("PipeServer::Init ,Error creating, named pipe: {} ,Errorcode : {}", Port, GetLastError());
		return false;
	}

	if (!ConnectNamedPipe(_Pipe, NULL))
	{
		LOGINFO("PipeServer::Init ,Error connecting to client, named pipe: {} ,Errorcode : {}", Port, GetLastError());
		CloseHandle(_Pipe);
		return 1;
	}
}

void PipeServer::Listen()
{
}

template <typename T>
void Pipe_Manager::SendTo_DS(PipeServer* Pipe, T& message)
{
	Header header;
	header.type = Get_Header_Type(message);
	if (header.type == 0)
		return;
	header.length = message.ByteSizeLong();

	char* content = new char[header.length + 1];
	message.SerializeToArray(content, header.length);

	PPIPE_IO_DATA pPerIO = (PPIPE_IO_DATA)::GlobalAlloc(GPTR, sizeof(PIPE_IO_DATA));
	memcpy(pPerIO->buf, &header, sizeof(header));
	memcpy(pPerIO->buf + sizeof(header), content, header.length);
	int sumlen = sizeof(header) + header.length;

	WriteFile(Pipe->_Pipe, pPerIO->buf, sumlen, &(pPerIO->dwResult), &pPerIO->ol);
}

void Pipe_Manager::Add_Pipe(PipeServer* Pipe) {

	CreateIoCompletionPort((HANDLE)Pipe->_Pipe, hIOCP, NULL, NumOfRecvThread);
	PPIPE_IO_DATA pPerIO = (PPIPE_IO_DATA)::GlobalAlloc(GPTR, sizeof(PIPE_IO_DATA));
	ReadFile(Pipe->_Pipe, pPerIO->buf, sizeof(pPerIO->buf), &(pPerIO->dwResult), &pPerIO->ol);
}

void Pipe_Manager::Del_Pipe(PipeServer* Pipe)
{
	CreateIoCompletionPort((HANDLE)Pipe->_Pipe, NULL, NULL, NumOfRecvThread);
}

bool Pipe_Manager::Init_PipeManager()
{
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);


	return true;
}
int Pipe_Manager::Run()
{
	Process_stop = false;

	//thread Send_Thread(&Pipe_Manager::Send_Process, this);
	//thread ProcesThreadpool[NumOfProcessThread]; // 申请多个处理线程(线程池)
	//for (int i = 0; i < NumOfProcessThread; i++)
		//ProcesThreadpool[i] = thread(&Pipe_Manager::Net_Process, this);
	thread RecvThread = thread(&Pipe_Manager::Pipe_Process, this);

	RecvThread.join();
	//for (int i = 0; i < NumOfProcessThread; i++)
		//ProcesThreadpool[i].join();
	//Send_Thread.join();

	ThreadEnd();
	LOGINFO("Pipe_Manager::Run ,Pipe_Manager Close!");
	return 1;
}
void Pipe_Manager::ThreadEnd()
{
	CloseHandle(hIOCP);
}
void Pipe_Manager::Pipe_Process()
{
	PPIPE_IO_DATA pPerIO = (PPIPE_IO_DATA)::GlobalAlloc(GPTR, sizeof(PIPE_IO_DATA));

	//Socket_Message* pinfo = nullptr;
	//int cur = 0;
	//int pack_flag = 0;	// 分包/黏包标志	0:正常 1:需要黏包(黏包起点为header)  2:需要黏包(黏包起点为content)

	unsigned long long pPerHandle;
	while (!Process_stop) {
		bool bl = ::GetQueuedCompletionStatus(hIOCP, &pPerIO->dwResult, (PULONG_PTR)&pPerHandle, (LPOVERLAPPED*)&pPerIO, 1000);
		if (!bl)
		{
			//closesocket(Socket);
			GlobalFree(pPerIO);
			break;
		}
		if (pPerIO->dwResult == 0 && (pPerIO->OperationType == OP_READ || pPerIO->OperationType == OP_WRITE)) {
			//closesocket(Socket);
			GlobalFree(pPerIO);
			break;
		}
		switch (pPerIO->OperationType)//通过per-IO数据中的nOperationType域查看有什么I/O请求完成了
		{
		case OP_READ:  //完成一个接收请求
		{
			PipeServer* Pipe = pPerIO->Pipe;
			if (!Pipe)return;

			Header header;
			char* content = nullptr;
			std::memcpy(&header, pPerIO->buf, sizeof(header));
			content = new char[header.length + 1];
			std::memcpy(content, pPerIO->buf + sizeof(header), header.length);

			//Game_Manager::Instance()->Process(Pipe, header, content);

			/* 重新投递重叠WSARecv请求 */
			PPIPE_IO_DATA pPerIO = (PPIPE_IO_DATA)::GlobalAlloc(GPTR, sizeof(PIPE_IO_DATA));
			ReadFile(Pipe->_Pipe, pPerIO, sizeof(pPerIO->buf), &(pPerIO->dwResult), &pPerIO->ol);
		}
		break;
		case OP_WRITE:
		{
			break;
		}
		}
	}
}
