#include "Net_Server.h"


SOCKET Get_NewSocket(std::string IP, uint16_t socket_port, int protocol, sockaddr_in& addr)
{
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(socket_port);
	if (IP != "")
		inet_pton(AF_INET, IP.c_str(), &(addr.sin_addr.s_addr));
	else
		addr.sin_addr.s_addr = INADDR_ANY;

	return WSASocket(addr.sin_family, protocol, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
}

template <typename T>
void Net_Server::SendTo_SendQueue(int socket, T& message)
{
	Header header;
	header.type = Get_Header_Type(message);
	if (header.type == 0)
		return;
	header.length = message.ByteSizeLong();

	Socket_Message* msg = new Socket_Message(socket, header);
	msg->content = new char[header.length + 1];
	message.SerializeToArray(msg->content, header.length);

	SendQueue.emplace(msg);      // 投递消息
	SendProcess_cv.notify_one(); // 唤醒一个发送线程
}

bool Net_Server::Init_NetServer()
{

	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	CreateIoCompletionPort((HANDLE)Socket, hIOCP, NULL, NumOfRecvThread);

	string IP = Config::Instance()->Read<string>("game_serverip", "0");
	int Port = Config::Instance()->Read("game_serverport", 0);
	Socket = Get_NewSocket(IP, Port, SOCK_STREAM, Socket_RemoteAddr);

	int flag = 1;
	setsockopt(Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));

	while (-1 == connect(Socket, (struct sockaddr*)&Socket_RemoteAddr, sizeof(struct sockaddr)))
	{
		LOGINFO("Net_Server::Init_NetServer , Connect Center Server Failed!");
		Sleep(3000);
	}
	unsigned long ul = 1;
	ioctlsocket(Socket, FIONBIO, &ul);

	return true;
}
int Net_Server::Run()
{
	Process_stop = false;

	thread Send_Thread(&Net_Server::Send_Process, this);
	thread ProcesThreadpool[NumOfProcessThread]; // 申请多个处理线程(线程池)
	for (int i = 0; i < NumOfProcessThread; i++)
		ProcesThreadpool[i] = thread(&Net_Server::Net_Process, this);
	thread RecvThread = thread(&Net_Server::Recv_Process, this);

	RecvThread.join();
	for (int i = 0; i < NumOfProcessThread; i++)
		ProcesThreadpool[i].join();
	Send_Thread.join();

	ThreadEnd();
	LOGINFO("Net_Server::Run ,Net_Server Close!");
	return 1;
}
void Net_Server::ThreadEnd()
{
	CloseHandle(hIOCP);
}
void Net_Server::Recv_Process()
{
	PPER_IO_DATA pPerIO = (PPER_IO_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_DATA));
	pPerIO->OperationType = OP_READ;
	DWORD dwBytesTranfered = 0;

	WSABUF buf;
	buf.buf = pPerIO->buf;
	buf.len = 1023;
	pPerIO->OperationType = OP_READ;
	DWORD nFlags = 0;
	::WSARecv(Socket, &buf, 1, &dwBytesTranfered, &nFlags, &pPerIO->ol, NULL);

	Socket_Message* pinfo = nullptr;
	int cur = 0;
	int pack_flag = 0;	// 分包/黏包标志	0:正常 1:需要黏包(黏包起点为header)  2:需要黏包(黏包起点为content)

	unsigned long long pPerHandle;
	while (!Process_stop) {
		bool bl = ::GetQueuedCompletionStatus(hIOCP, &dwBytesTranfered, (PULONG_PTR)&pPerHandle, (LPOVERLAPPED*)&pPerIO, 1000);
		if (!bl)
		{
			closesocket(Socket);
			GlobalFree(pPerIO);
			break;
		}
		if (dwBytesTranfered == 0 && (pPerIO->OperationType == OP_READ || pPerIO->OperationType == OP_WRITE)) {
			closesocket(Socket);
			GlobalFree(pPerIO);
			break;
		}
		switch (pPerIO->OperationType)
		{   //通过per-IO数据中的nOperationType域查看有什么I/O请求完成了
		case OP_READ:  //完成一个接收请求
		{
			// dwBytesTranfered表示接收到的字节数，cur表示缓冲区剩余字节数（因为需要黏包而残留在缓冲区中）
			// 新的dwBytesTranfered表示缓冲区总共字节数
			dwBytesTranfered = dwBytesTranfered + cur;
			cur = 0;

			// 粘包标志不为0，表示需要黏包
			if (pack_flag == 1 && dwBytesTranfered - cur >= sizeof(Header))
			{
				pinfo = new Socket_Message();
				pinfo->SetHeader(&(pPerIO->buf[cur]));

				cur += sizeof(Header);
				pack_flag = 0;
			}
			else if (pack_flag == 2 && dwBytesTranfered - cur >= pinfo->header.length)
			{
				pinfo->SetContent(&(pPerIO->buf[cur]));
				cur += pinfo->header.length;
				unique_lock<mutex> Queuelck(RecvQueue_mtx);
				RecvQueue.push(pinfo);
				Queuelck.release()->unlock();
				NetProcess_cv.notify_one();
				pinfo = nullptr;
				pack_flag = 0;
			}


			while (!pack_flag)
			{
				// pinfo为空则获取包头，若不为空（说明包头在黏包过程中获取了，只需要再获取内容就好了）
				if (!pinfo)
				{
					// 剩余字符已经不够组成新的头部，需要黏包
					if (dwBytesTranfered - cur < sizeof(Header)) {
						// 把剩余字符移动至buf起始位置
						memmove(pPerIO->buf, &(pPerIO->buf[cur]), dwBytesTranfered - cur);
						cur = dwBytesTranfered - cur;
						pack_flag = 1;
						break;
					}
					else {
						//获取头
						pinfo = new Socket_Message();
						pinfo->SetHeader(&(pPerIO->buf[cur]));

						//{		// 测试用控制台输出
						//	wstring wstr = to_wstring(pinfo->header.type) + L"\n";
						//	wchar_t szOutputTest[MAX_PATH];
						//	lstrcpy(szOutputTest, wstr.c_str());
						//	DWORD dwStringLength = wcslen(szOutputTest);
						//	DWORD dwBytesWritten = 0;
						//	DWORD dwErrorCode = 0;
						//	WriteConsole(hStdout, szOutputTest, dwStringLength, &dwBytesWritten, NULL);
						//}

						cur += sizeof(Header);
					}
				}

				// 剩余字符已经不够组成新的头部，需要黏包
				if (dwBytesTranfered - cur < pinfo->header.length)
				{
					// 把剩余字符移动至buf起始位置
					memmove(pPerIO->buf, &(pPerIO->buf[cur]), dwBytesTranfered - cur);
					cur = dwBytesTranfered - cur;
					pack_flag = 2;
					break;
				}
				else {
					pinfo->SetContent(&(pPerIO->buf[cur]));
					cur += pinfo->header.length;
					unique_lock<mutex> Queuelck(RecvQueue_mtx);
					RecvQueue.push(pinfo);
					Queuelck.release()->unlock();
					NetProcess_cv.notify_one();
					pinfo = nullptr;
				}

				// 相等则说明不需要黏包
				if (cur == dwBytesTranfered) {
					cur = 0;
					break;
				}
			}

			/* 重新投递重叠WSARecv请求 */
			WSABUF buf;
			buf.buf = &(pPerIO->buf[cur]);
			buf.len = 1023 - cur;
			pPerIO->OperationType = OP_READ;
			DWORD nFlags = 0;
			::WSARecv(Socket, &buf, 1, &dwBytesTranfered, &nFlags, &pPerIO->ol, NULL);
		}
		break;
		case OP_WRITE:break;
		}
	}
}
void Net_Server::Net_Process()
{
	Socket_Message* Recv_Message = nullptr;
	while (!Process_stop)
	{
		unique_lock<mutex> Processlck(NetProcess_mtx);
		NetProcess_cv.wait(Processlck);
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

			int ret = OnProcess(Recv_Message);
		}
	}
}
void Net_Server::Send_Process()
{
	Socket_Message* Send_Message = nullptr;
	while (!Process_stop)
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

			char* sendbuf = new char[sendlen];
			memset(sendbuf, '\0', sendlen);
			memcpy(sendbuf, &(Send_Message->header), sizeof(Header));
			memcpy(sendbuf + sizeof(Header), Send_Message->content, Send_Message->header.length);

			send(Send_Message->socket_fd, sendbuf, sendlen, 0);
			delete sendbuf;
		}
	}
}

int Net_Server::OnProcess(Socket_Message* msg)
{
	const Header& header = msg->header;
	const int socket_fd = msg->socket_fd;
	char* content = msg->content;

	switch (header.type)
	{
		//case ping_Request:
		//{
		//	OnPing(socket_fd, header, content);
		//	break;
		//}
		//case Request_SendPublicMessage:
		//{
		//	Game_Manager::Instance()->Process(msg);
		//	break;
		//}
	}
	delete msg;
	msg = nullptr;

	return 1;
}

void Net_Server::OnPing(const int socket_fd, const Header header, const char* content)
{
	//Hall_Protobuf::Ping_Request Request;
	//Request.ParseFromArray(content, header.length);

	//Hall_Protobuf::Ping_Response Response;
	//Response.set_pingpack_id(Request.pingpack_id());

	//SendTo_SendQueue(socket_fd, Response);
}
