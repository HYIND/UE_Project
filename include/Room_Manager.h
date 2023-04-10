#pragma once

#include "header.h"
#include "Center_Server.h"
#include "RPC_Server.h"

enum class CreateRoom_Result
{
    Fail = 0,
    Success = 1
};

enum class JoinRoom_Result
{
    RoomNotFound = -1,
    PlayerLimit = 0,
    Success = 1
};

enum class ExitRoom_Result
{
    Fail = 0,
    Success = 1,
    SWitch_Host = 2,
};

enum class KickRoom_Result
{
    Fail = 0,
    Success = 1
};

enum class StartGame_Result
{
    InnerError = -1,
    FailCauseReady = 0,
    Success = 1
};

enum class Ready_State
{
    NoReady = 0,
    Ready = 1
};

enum class Groups
{
    Red = 0,
    Blue = 1,
    Green = 2,
    Yellow = 3
};

enum class NewGame_Result
{
    Fail = 0,
    Success = 1
};

enum class Map
{
    Island = 0,
    Continent = 1
};

using namespace std;

class Room_Manager;

struct PlayerInfo
{
    User_Info userinfo;
    Ready_State state;
    Groups group;

    PlayerInfo(User_Info &user)
    {
        userinfo = user;
        state = Ready_State::NoReady;
        group = Groups::Blue;
    }
};

struct Room_Info
{
    int room_id;
    int host_id;
    string host_name;
    int playersnum;
    int playerslimit;
    Map mapid;

    vector<PlayerInfo> playerinfo;

    int room_state;
    bool room_lock;

    vector<PlayerInfo> Get_player() { return playerinfo; }
    vector<User_Info> Get_player_Userinfo()
    {
        vector<User_Info> result;
        for (auto &player : playerinfo)
            result.emplace_back(player.userinfo);
        return result;
    }
    vector<User_Info> Get_player_Userinfo_nohost()
    {
        vector<User_Info> result;
        for (auto &player : playerinfo)
        {
            if (player.userinfo.Get_ID() != host_id)
                result.emplace_back(player.userinfo);
        }
        return result;
    }

    void Add_player(User_Info &userinfo)
    {
        PlayerInfo player(userinfo);
        playerinfo.emplace_back(player);
        playersnum++;
    }
    bool Exit_player(int id)
    {
        for (auto it = playerinfo.begin(); it != playerinfo.end(); it++)
        {
            if ((*it).userinfo.Get_ID() == id)
            {
                playerinfo.erase(it);
                playersnum--;
                return true;
            }
        }

        return false;
    }
    bool ChangeMap(Map id)
    {
        mapid = id;
        return true;
    }
    bool Changelimit(int limit)
    {
        if (playersnum > playerslimit)
            return false;
        playerslimit = limit;
        return true;
    }
    bool Changehost(int id)
    {
        for (auto &player : playerinfo)
        {
            if (player.userinfo.Get_ID() == id)
            {

                host_id = id;
                host_name = player.userinfo.Get_UserName();
                LOGINFO("Room_Info::Changehost roomid : {} , hostname : {}", room_id, host_name);
                return true;
            }
        }
        return false;
    }
    bool ChangeReadyState(int id, Ready_State &state_out)
    {
        for (auto &player : playerinfo)
        {
            if (player.userinfo.Get_ID() == id)
            {
                if (player.state == Ready_State::NoReady)
                    player.state = Ready_State::Ready;
                else
                    player.state = Ready_State::NoReady;

                state_out = player.state;
                LOGINFO("Room_Info::ChangeReadyState roomid : {} , playername : {} , state : {}", room_id, player.userinfo.Get_UserName(), (int)player.state);
                return true;
            }
        }
        return false;
    }
    bool ChangeTeam(int id, Groups &group_out)
    {
        for (auto &player : playerinfo)
        {
            if (player.userinfo.Get_ID() == id)
            {
                if (player.group == Groups::Blue)
                    player.group = Groups::Red;
                else
                    player.group = Groups::Blue;

                group_out = player.group;
                LOGINFO("Room_Info::ChangeTeam roomid : {} , playername : {} , group : {}", room_id, player.userinfo.Get_UserName(), (int)player.group);
                return true;
            }
        }
        return false;
    }
    void Lock() { room_lock = true; }
    void UnLock() { room_lock = false; }
};

class Room_Manager
{
public:
    static Room_Manager *Instance()
    {
        static Room_Manager *m_Instance = new Room_Manager();
        return m_Instance;
    }

public:
    int Process(Socket_Message *msg);

    void OnCreateRoom(const int socket_fd, const Header header, const char *content);
    void OnSerachRoom(const int socket_fd, const Header header, const char *content);
    void OnJoinRoom(const int socket_fd, const Header header, const char *content);
    void OnExitRoom(const int socket_fd, const Header header, const char *content);
    void OnKickRoom(const int socket_fd, const Header header, const char *content);
    void OnChangeMap(const int socket_fd, const Header header, const char *content);
    void OnChangelimit(const int socket_fd, const Header header, const char *content);
    void OnChangeReady(const int socket_fd, const Header header, const char *content);
    void OnChangeTeam(const int socket_fd, const Header header, const char *content);
    void OnSendRoomMessage(const int socket_fd, const Header header, const char *content);

    void OnStartGame(const int socket_fd, const Header header, const char *content);

protected:
    shared_ptr<Room_Protobuf::SerachRoom_Response> Get_RoomList_ProtobufInfo();
    CreateRoom_Result CreateRoom(User_Info *userinfo, const int playerslimit, const Map mapid);
    JoinRoom_Result JoinRoom(User_Info *userinfo, const int room_id, Room_Info *&Info);
    ExitRoom_Result ExitRoom(User_Info *userinfo, vector<User_Info> &remain_player, Room_Info *&room_out);
    KickRoom_Result KickRoom(User_Info *userinfo, int killid, vector<User_Info> &remain_player);
    StartGame_Result StartGame(User_Info *userinfo, vector<User_Info> &remain_player, string &ds_ip);
    bool ChangeReadyState(User_Info *userinfo, Ready_State state_in, vector<User_Info> &remain_player);
    bool ChangeMap(User_Info *userinfo, Map mapid, vector<User_Info> &remain_player);
    bool Changelimit(User_Info *userinfo, int limit, vector<User_Info> &remain_player);
    bool ChangeReady(User_Info *userinfo, Ready_State &State_out, vector<User_Info> &remain_player);
    bool ChangeTeam(User_Info *userinfo, Groups &group_out, vector<User_Info> &remain_player);

    bool Get_RoomPlayer(int user_id, vector<PlayerInfo> &playerinfo);
    bool RemoveRoom(Room_Info *room);

private:
    Room_Manager();

public:
    vector<Room_Info *> Room_list;
    map<int, Room_Info *> Quick_map; //  userid->Room
    int roomid_count = 0;

private:
    bool RoomChanged;
    shared_ptr<Room_Protobuf::SerachRoom_Response> Probuf_SharePtr;
};