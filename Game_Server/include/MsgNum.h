#pragma once
#include "Game_Protocol.pb.h"
#include "DS_Protocol.pb.h"

enum Game_Server_MsgNum
{
	Request_NewGame = 4000,

	Response_NewGame = 5000
};

enum DS_MsgNum
{
	Request_NewDSGame = 7000,

	Response_NewDSGame = 7010
};

template <typename T>
int Get_Header_Type(T& message)
{
	if (is_same<T, Game_Protobuf::NewGame_Request>::value)
		return Game_Server_MsgNum::Request_NewGame;
	if (is_same<T, Game_Protobuf::NewGame_Response>::value)
		return Game_Server_MsgNum::Response_NewGame;

	if (is_same<T, DS_Protobuf::NewDSGame_Request>::value)
		return DS_MsgNum::Request_NewDSGame;
	if (is_same<T, DS_Protobuf::NewDSGame_Response>::value)
		return DS_MsgNum::Response_NewDSGame;

	return 0;
}