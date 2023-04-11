#pragma once
#include "stdafx.h"
#include "NetworkBase.h"
#include "RPC_Server.h"

using namespace std;


enum class NewGame_Result
{
	Fail = 0,
	Success = 1
};

enum class DSState_Result
{
	Fail = 0,
	Success = 1
};

enum class Map
{
	Island = 0,
	Continent = 1
};

enum class Groups
{
	Red = 0,
	Blue = 1,
	Green = 2,
	Yellow = 3
};

struct Game_PlayerInfo
{
	int id;
	string name;
	string token;
	Groups group;
	bool isJoin;

	Game_PlayerInfo(int id_in, const string& name_in, const string &token_in, Groups group_in)
		: id(id_in), name(name_in), group(group_in)
	{
		token = token_in;
		isJoin = false;
	}

	int Get_ID() const { return id; }
	string Get_UserName() const { return name; }
	string Get_Token() { return token; }
	Groups Get_Group() { return group; }
};

struct Game_Info
{
	int game_id;
	int room_id;

	int host_id;
	int playersnum;
	int playerslimit;
	Map mapid;

	vector<Game_PlayerInfo> playerinfo;

	string DS_IP;
	int Port;

	Game_Info(int roomid_in, int gameid_in, int hostid_in, int playersnum_in, int playerslimit_in, Map mapid_in, string& DS_IP_in, int DS_Port_in)
		:room_id(roomid_in), game_id(gameid_in), host_id(hostid_in), playersnum(playersnum_in), playerslimit(playerslimit_in), mapid(mapid_in), DS_IP(DS_IP), Port(DS_Port_in) {}

	int Get_GameID() { return game_id; }
	int Get_RoomID() { return room_id; }
	int Get_HostID() { return host_id; }
	int Get_PlayersNum() { return playersnum; }
	int Get_PlayersLimit() { return playerslimit; }
	Map Get_MapId() { return mapid; }
	vector<Game_PlayerInfo>& Get_Player() { return playerinfo; }
	string Get_GameIP() { return DS_IP; }
	int Get_GamePort() { return Port; }

	void AddPlayer(const Game_PlayerInfo& player) { playerinfo.push_back(player); }
	void AddPlayer(int id_in, const string& name_in, string token_in, Groups group_in) { playerinfo.emplace_back(id_in, name_in, token_in, group_in); }
};

class Game_Manager
{
public:
	static Game_Manager* Instance()
	{
		static Game_Manager* m_Instance = new Game_Manager();
		return m_Instance;
	}

public:
	int Process(Header header, const char* content);
	void OpenGame(Game_Protobuf::NewGame_Request& Request, Game_Protobuf::NewGame_Response& Response);
protected:
	bool Confirm_DSGameState(Game_Info& game);

private:
	Game_Manager() {};
	void AddGame(Game_Info* game);
	void DelGame(Game_Info* game);

private:
	vector<Game_Info*> Game_list;
	map<int, Game_Info*> QuickUserID_map;//  userid->Room
	map<int, int> Room_Game_map;		 //  Roomid->Gameid
	map<int, Game_Info*> QuickGameID_map;//  gameid->Game
	int gameid_count = 1;

};