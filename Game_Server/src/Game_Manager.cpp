#pragma once
#include "Game_Manager.h"

int Game_Manager::Process(Header header, const char* content)
{
	return 1;
}

void Game_Manager::OpenGame(Game_Protobuf::NewGame_Request& Request, Game_Protobuf::NewGame_Response& Response)
{
	static int Port = 8888;
	static string GamePublicIP = "120.79.17.70";
	static string GameListenIP = "172.19.4.108";

	static string command =
		"Release" == Config::Instance()->Read<string>("DS_runmode", "Release") ?
		"start ./Release/WindowsServer/myprojectServer.exe -log -multihome=" + GameListenIP + " -port=" :
		"start ./Devlopment/WindowsServer/myprojectServer.exe -log -multihome=" + GameListenIP + " -port=";

	string strPort = to_string(Port);
	system((command + strPort).c_str());
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	Game_Protobuf::Gameinfo proto_game = Request.gameinfo();
	Game_Info* game = new Game_Info(proto_game.roomid(), gameid_count++, proto_game.hostid(), proto_game.playersnum(), proto_game.playerslimit(), (Map)proto_game.mapid(), GamePublicIP, Port);
	for (int i = 0; i < proto_game.playersallinfo_size(); i++)
	{
		Game_Protobuf::PlayerAllinfo proto_player = proto_game.playersallinfo(i);
		game->AddPlayer(proto_player.id(), proto_player.playersname(), proto_player.token(), (Groups)proto_player.group());
	}

	if (!Confirm_DSGameState(*game))
	{
		Response.set_result((int)NewGame_Result::Fail);
		LOGERROR("RPC_Server::RPC_OpenGame Request OpenGame Fail!");
		Port++;
		return;
	}

	AddGame(game);

	string DS_IP_Port = GamePublicIP + ":" + strPort;
	Response.set_result((int)NewGame_Result::Success);
	Response.set_ds_ip(DS_IP_Port);

	Port++;
	LOGINFO("RPC_Server::RPC_OpenGame Request OpenGame Success! DS_IP : {}", DS_IP_Port);
}

bool Game_Manager::Confirm_DSGameState(Game_Info& game)
{
	DS_Protobuf::DSGameinfo* proto_DSGameinfo = new DS_Protobuf::DSGameinfo();
	proto_DSGameinfo->set_hostid(game.host_id);
	proto_DSGameinfo->set_mapid((int)game.mapid);
	proto_DSGameinfo->set_playersnum(game.playersnum);
	proto_DSGameinfo->set_playerslimit(game.playerslimit);
	for (auto player : game.Get_Player())
	{
		DS_Protobuf::DSPlayerinfo* proto_DSPlayerinfo = proto_DSGameinfo->add_playersinfo();
		proto_DSPlayerinfo->set_id(player.Get_ID());
		proto_DSPlayerinfo->set_playersname(player.Get_UserName());
		proto_DSPlayerinfo->set_token(player.Get_Token());
		proto_DSPlayerinfo->set_group((int)player.Get_Group());
	}

	DS_Protobuf::NewDSGame_Request DS_Request;
	DS_Protobuf::NewDSGame_Response DS_Response;
	DS_Request.set_allocated_gameinfo(proto_DSGameinfo);

	if (!RPC_Client::Excute_RPCRequest(DS_Request, DS_Response, "127.0.0.1", game.Get_GamePort(), 3))
		return false;

	if (DS_Response.result() == (int)DSState_Result::Fail)
		return false;

	return true;
}

void Game_Manager::AddGame(Game_Info* game)
{
	Game_list.emplace_back(game);
	Room_Game_map[game->room_id] = game->game_id;
	QuickGameID_map[game->Get_GameID()] = game;
	for (auto player : game->Get_Player())
		QuickUserID_map[player.Get_ID()] = game;
}

void Game_Manager::DelGame(Game_Info* game)
{

}