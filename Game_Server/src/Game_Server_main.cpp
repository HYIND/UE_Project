// Game_Server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "stdafx.h"
#include "Net_Server.h"
#include "RPC_Server.h"
#include "Game_Manager.h"

using namespace std;

int Init_Server()
{
	LOGINFO("Start Game Server!");

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//Net_Server::Instance()->Init_NetServer();
	RPC_Server::Instance()->Init();


	//thread T1(&Net_Server::Run, Net_Server::Instance());
	thread T2(&RPC_Server::Start, RPC_Server::Instance());


	//test
	//thread T3(
	//	[]()
	//	{
	//		Game_Protobuf::NewGame_Request Request;
	//		Game_Protobuf::NewGame_Response Response;
	//		RPC_Client::Instance()->Excute_RPCRequest(Request, Response, RPC_GameServer);
	//	});

	//T1.join();
	T2.join();
	//T3.join();

	WSACleanup();
	return true;
}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	Init_Server();
	return 0;
}


