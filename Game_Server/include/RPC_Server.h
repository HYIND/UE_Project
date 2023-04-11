#pragma once

#include "stdafx.h"
#include "NetworkBase.h"

class RPC_Server
{
public:
	static RPC_Server* Instance()
	{
		static RPC_Server* m_Instance = new RPC_Server();
		return m_Instance;
	}
private:
	RPC_Server() {}

public:
	void Init();
	void Start();

protected:
	int OnProcess(RPC_Message& msg);

	template<class _Request, class _Response>
	void _Dispatche(RPC_Message& Msg, void (*Fun)(_Request&, _Response&));
	template<class _Request, class _Response, class _Instance>
	void _Dispatche(RPC_Message& Msg, void(_Instance::* Fun)(_Request&, _Response&), _Instance* instance);


private:
	SOCKET Server_Socket;
	sockaddr_in Server_Addr;
};

enum RCP_DstServer
{
	RPC_CenterServer = 1,
	RPC_GameServer = 2
};

class RPC_Client
{
public:
	static RPC_Client* Instance()
	{
		static RPC_Client* m_Instance = new RPC_Client();
		return m_Instance;
	}
private:
	RPC_Client() {}

protected:
	static bool Excute_SocketRequest(RPC_Message& Msg, char* re_buf, int* re_num);

public:
	template <typename T1, typename T2>
	static bool Excute_RPCRequest(T1& Request, T2& Response, RCP_DstServer Dst_Server, int trycount = 1)
	{
		RPC_Message Msg;
		string IP;
		int Port = 0;
		switch (Dst_Server)
		{
		case RPC_CenterServer:
			IP = Config::Instance()->Read<string>("center_rpcip", "0");
			Port = Config::Instance()->Read("center_rpcport", 0);
			break;

		default:
			LOGERROR("RPC_Client::SendRPC_Request: Dst_Server wrong!");
			return false;
		}
		if (IP == "0" || Port == 0)
		{
			LOGERROR("RPC_Client::SendRPC_Request: Server {} config wrong!", (int)Dst_Server);
			return false;
		}

		Msg.Fill_RemoteAddrInfo(IP, Port);
		Msg.Fill_RequestInfo(Request);

		char re_buf[4096];
		int re_num = 0;

		while (!Excute_SocketRequest(Msg, re_buf, &re_num))
		{
			trycount--;
			if (trycount <= 0)
				return false;
		}

		if (!Msg.ParseResponseBuf(re_buf, re_num))
			return false;

		Response.ParseFromArray(Msg.Response_buffer, Msg.Response_header.length);
		return true;
	}

	template <typename T1, typename T2>
	static bool Excute_RPCRequest(T1& Request, T2& Response, string IP, int Port, int trycount = 1)
	{

		if (IP == "0" || Port == 0)
		{
			LOGERROR("RPC_Client::SendRPC_Request: Server config wrong : {}:{}", IP, Port);
			return false;
		}

		RPC_Message Msg;
		Msg.Fill_RemoteAddrInfo(IP, Port);
		Msg.Fill_RequestInfo(Request);

		char re_buf[4096];
		int re_num = 0;

		while (!Excute_SocketRequest(Msg, re_buf, &re_num))
		{
			trycount--;
			if (trycount <= 0)
				return false;
		}

		if (!Msg.ParseResponseBuf(re_buf, re_num))
			return false;

		Response.ParseFromArray(Msg.Response_buffer, Msg.Response_header.length);
		return true;
	}

};

