#include "RPC_Server.h"
#include "Game_Manager.h"

int RPC_Server::OnProcess(RPC_Message& Msg)
{
	switch (Msg.Request_header.type)
	{
	case Request_NewGame:
		_Dispatche(Msg, &Game_Manager::OpenGame, Game_Manager::Instance());
		break;

	default:
		break;
	}

	return 1;
}

void RPC_Server::Init()
{
	string IP = Config::Instance()->Read<string>("game_rpcip", "0");
	int Port = Config::Instance()->Read("game_rpcport", 0);
	ZeroMemory(&Server_Addr, sizeof(Server_Addr));
	Server_Addr.sin_family = AF_INET;
	Server_Addr.sin_port = htons(Port);
	inet_pton(AF_INET, IP.c_str(), &(Server_Addr.sin_addr.s_addr));
	Server_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == ::bind(Server_Socket, (struct sockaddr*)&Server_Addr, sizeof(Server_Addr)))
	{
		perror("RPC_Server::Init bind socket error");
		LOGINFO("RPC_Server::Init bind socket error");
		return;
	}
	LOGINFO("RPC_Server::Init : IP : {},Port : {}", IP, Port);
}

void RPC_Server::Start()
{
	while (true)
	{
		RPC_Message RPC_Msg;
		socklen_t from_len = sizeof(RPC_Msg.remote_addr);
		char re_buf[4096];
		LOGINFO("RPC_Server::Start Waiting...");
		int re_num = recvfrom(Server_Socket, re_buf, 4095, 0, (struct sockaddr*)&RPC_Msg.remote_addr, &from_len);
		if (re_num < 0)
			continue;
		else if (re_num >= (int)(TokenSize + sizeof(Header)))
		{
			char IP[20];
			inet_ntop(AF_INET, &(RPC_Msg.remote_addr.sin_addr.s_addr), IP, 20);
			LOGINFO("RPC_Server::Start Recvfrom {}:{}", IP, ntohs(RPC_Msg.remote_addr.sin_port));
			if (!RPC_Msg.ParseRequestBuf(re_buf, re_num))
				continue;

			OnProcess(RPC_Msg);

			if (RPC_Msg.Response_header.type != 0 && RPC_Msg.Response_buffer)
			{
				char* sum_sendbuf = nullptr;
				int sum_sendlen = 0;
				if (RPC_Msg.GenerateResponseBuf(sum_sendbuf, &sum_sendlen) != false)
					sendto(Server_Socket, sum_sendbuf, sum_sendlen, 0, (struct sockaddr*)&RPC_Msg.remote_addr, from_len);
				if (sum_sendbuf)
					delete sum_sendbuf;
			}
		}
	}
}


template<class _Request, class _Response>
void RPC_Server::_Dispatche(RPC_Message& Msg, void (*Fun)(_Request&, _Response&))
{
	_Request Request;
	_Response Response;
	Request.ParseFromArray(Msg.Request_buffer, Msg.Request_header.length);

	*Fun(Request, Response);

	Msg.Fill_ResponseInfo(Response);
}

template<class _Request, class _Response, class _Instance>
void RPC_Server::_Dispatche(RPC_Message& Msg, void(_Instance::* Fun)(_Request&, _Response&), _Instance* instance)
{
	_Request Request;
	_Response Response;
	Request.ParseFromArray(Msg.Request_buffer, Msg.Request_header.length);

	if (instance != nullptr)
		(instance->*Fun)(Request, Response);

	Msg.Fill_ResponseInfo(Response);
}

bool RPC_Client::Excute_SocketRequest(RPC_Message& Msg, char* re_buf, int* re_num)
{
	assert(re_buf != nullptr && re_num != nullptr);

	timeval tv = { 5, 0 };
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	SOCKET socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
	setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval));

	int sum_sendlen = 0;
	char* sum_Request_buf;
	Msg.GenerateRequestBuf(sum_Request_buf, &sum_sendlen);
	int send_num = sendto(socket_fd, sum_Request_buf, sum_sendlen, 0, (struct sockaddr*)&Msg.remote_addr, sizeof(Msg.remote_addr));
	if (sum_Request_buf)
		delete sum_Request_buf;
	if (send_num <= 0)
	{
		perror("RPC_Client::Excute_SocketRequest :Request wrong!");
		LOGERROR("RPC_Client::Excute_SocketRequest :Request wrong!");
		closesocket(socket_fd);
		return false;
	}

	socklen_t from_len = sizeof(Msg.remote_addr);
	*re_num = recvfrom(socket_fd, re_buf, 4095, 0, (struct sockaddr*)&Msg.remote_addr, &from_len);

	if (*re_num <= 0)
	{
		perror("RPC_Client::Excute_SocketRequest :No Response!");
		LOGERROR("RPC_Client::Excute_SocketRequest :No Response!");
		closesocket(socket_fd);
		return false;
	}

	closesocket(socket_fd);
	return true;
}