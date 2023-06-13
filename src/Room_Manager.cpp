#include "Room_Manager.h"
#include "MsgNum.h"

Room_Manager::Room_Manager()
{
    RoomChanged = false;
    Probuf_SharePtr = make_shared<Room_Protobuf::SerachRoom_Response>();
}

int Room_Manager::Process(Socket_Message *msg)
{
    const Header &header = msg->header;
    const int socket_fd = msg->socket_fd;
    char *content = msg->content;

    switch (header.type)
    {
    case Request_CreateRoom:
        OnCreateRoom(socket_fd, header, content);
        break;
    case Request_SerachRoom:
        OnSerachRoom(socket_fd, header, content);
        break;
    case Request_JoinRoom:
        OnJoinRoom(socket_fd, header, content);
        break;
    case Request_ExitRoom:
        OnExitRoom(socket_fd, header, content);
        break;
    case Request_KickRoom:
        OnKickRoom(socket_fd, header, content);
        break;
    case Request_SendRoomMessage:
        OnSendRoomMessage(socket_fd, header, content);
        break;
    case Request_ChangeMap:
        OnChangeMap(socket_fd, header, content);
        break;
    case Request_Changelimit:
        OnChangelimit(socket_fd, header, content);
        break;
    case Request_ChangeReadyState:
        OnChangeReady(socket_fd, header, content);
        break;
    case Request_ChangeTeam:
        OnChangeTeam(socket_fd, header, content);
        break;
    case Request_StartGame:
        OnStartGame(socket_fd, header, content);
        break;
    case Request_RoomInfo:
        OnRoomInfo(socket_fd, header, content);
    }

    return 1;
}

void Room_Manager::OnCreateRoom(const int socket_fd, const Header header, const char *content)
{
    Room_Protobuf::CreateRoom_Request Request;
    Request.ParseFromArray(content, header.length);

    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);
    if (!user)
        return;

    Room_Protobuf::CreateRoom_Response Response;
    Response.set_result((int)CreateRoom(user, Request.createinfo().playerslimit(), (Map)Request.createinfo().mapid()));

    if (Response.result() == (int)CreateRoom_Result::Success)
        user->states = USER_STATE::Room;

    Center_Server::Instance()->SendTo_SendQueue(socket_fd, Response);
}

void Room_Manager::OnSerachRoom(const int socket_fd, const Header header, const char *content)
{
    shared_ptr<Room_Protobuf::SerachRoom_Response> RoomList_SharePtr = Get_RoomList_ProtobufInfo();
    Room_Protobuf::SerachRoom_Response *Response = RoomList_SharePtr.get();

    Center_Server::Instance()->SendTo_SendQueue(socket_fd, *Response);
    LOGINFO("Room_Manager::OnSerachRoom user serachroom : {}", Center_Server::Instance()->Get_Userinfo(socket_fd)->Get_UserName());
}

void Room_Manager::OnJoinRoom(const int socket_fd, const Header header, const char *content)
{

    Room_Protobuf::JoinRoom_Request Request;
    Request.ParseFromArray(content, header.length);
    int id = Request.roomid();

    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);
    if (!user)
        return;

    Room_Protobuf::JoinRoom_Response Response;
    Room_Info *room;
    Response.set_result((int)JoinRoom(user, id, room));

    if (Response.result() == (int)JoinRoom_Result::Success)
    {
        Room_Protobuf::RoomAllInfo *roominfo = new Room_Protobuf::RoomAllInfo();
        roominfo->set_host_name(room->host_name);
        roominfo->set_playersnum(room->playersnum);
        roominfo->set_playerslimit(room->playerslimit);
        roominfo->set_mapid((int)room->mapid);
        Response.set_allocated_roominfo(roominfo);

        vector<int> Vec_Mutisocket;
        Room_Protobuf::JoinRoom_MutiResponse MutiResoponse;

        for (auto player : room->Get_player())
        {
            if (player.user->Get_ID() == user->Get_ID())
            {
                Room_Protobuf::Playerinfo *proto_playerinfo = new Room_Protobuf::Playerinfo();
                proto_playerinfo->set_playersname(player.user->Get_UserName());
                proto_playerinfo->set_state((int)player.state);
                proto_playerinfo->set_group((int)player.group);
                MutiResoponse.set_allocated_playerinfo(proto_playerinfo);
            }
            else
                Vec_Mutisocket.push_back(player.user->Get_SocketFd());
        }

        for (auto player : room->Get_player())
        {
            Room_Protobuf::Playerinfo *proto_playerinfo = roominfo->add_playerinfo();
            proto_playerinfo->set_playersname(player.user->Get_UserName());
            proto_playerinfo->set_state((int)player.state);
            proto_playerinfo->set_group((int)player.group);
        }
        Center_Server::Instance()->SendMutiTo_SendQueue(Vec_Mutisocket, MutiResoponse);
        user->states = USER_STATE::Room;
    }
    Center_Server::Instance()->SendTo_SendQueue(socket_fd, Response);
}

void Room_Manager::OnExitRoom(const int socket_fd, const Header header, const char *content)
{
    Room_Protobuf::ExitRoom_Response Response;
    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);
    if (!user)
        return;
    user->states = USER_STATE::Hall;

    vector<User_Info *> Remain_Player_Userinfo;
    Room_Info *room;
    ExitRoom_Result result = ExitRoom(user, Remain_Player_Userinfo, room);

    Response.set_result((int)result);
    Center_Server::Instance()->SendTo_SendQueue(socket_fd, Response);

    if (Remain_Player_Userinfo.size() <= 0)
        return;

    Room_Protobuf::Playerinfo *proto_player = new Room_Protobuf::Playerinfo();
    proto_player->set_playersname(user->Get_UserName());

    Room_Protobuf::ExitRoom_MutiResponse MutiRespsone;
    MutiRespsone.set_allocated_playerinfo(proto_player);

    if (result == ExitRoom_Result::SWitch_Host)
    {
        MutiRespsone.set_changehost(1);
        MutiRespsone.set_new_hostname(room->host_name);
    }
    else
        MutiRespsone.set_changehost(0);

    vector<int> Vec_socketfd;
    for (auto fuser : Remain_Player_Userinfo)
        Vec_socketfd.push_back(fuser->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socketfd, MutiRespsone);
}

void Room_Manager::OnKickRoom(const int socket_fd, const Header header, const char *content)
{
    Room_Protobuf::KickRoom_Request Request;
    Request.ParseFromArray(content, header.length);

    string kickedName = Request.playerinfo().playersname();

    int kickid = -1;
    int kick_socket = -1;
    User_Info *user = nullptr;
    for (auto fuser : Center_Server::Instance()->Get_user_list())
    {
        if (fuser->Get_UserName() == kickedName)
        {
            kickid = fuser->Get_ID();
            kick_socket = fuser->Get_SocketFd();
            user = fuser;
            break;
        }
    }
    if (!user)
        return;

    user->states = USER_STATE::Hall;
    vector<User_Info *> Remain_Player_Userinfo;

    if (KickRoom_Result::Fail == KickRoom(user, kickid, Remain_Player_Userinfo))
        return;

    Room_Protobuf::KickRoom_Response Response;
    Center_Server::Instance()->SendTo_SendQueue(kick_socket, Response);

    Room_Protobuf::Playerinfo *proto_kickedplayer = new Room_Protobuf::Playerinfo();
    proto_kickedplayer->set_playersname(kickedName);

    Room_Protobuf::ExitRoom_MutiResponse MutiRespsone;
    MutiRespsone.set_allocated_playerinfo(proto_kickedplayer);
    vector<int> Vec_socketfd;
    for (auto fuser : Remain_Player_Userinfo)
        Vec_socketfd.push_back(fuser->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socketfd, MutiRespsone);
}

void Room_Manager::OnChangeMap(const int socket_fd, const Header header, const char *content)
{
    Room_Protobuf::ChangeMap_Request Request;
    Request.ParseFromArray(content, header.length);

    Map mapid = (Map)Request.mapid();
    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);
    if (!user)
        return;

    vector<User_Info *> Remain_Player_Userinfo;

    if (false == ChangeMap(user, mapid, Remain_Player_Userinfo))
        return;

    Room_Protobuf::ChangeMap_MutiResponse MutiRespsone;
    MutiRespsone.set_mapid((int)mapid);
    vector<int> Vec_socketfd;
    for (auto fuser : Remain_Player_Userinfo)
        Vec_socketfd.push_back(fuser->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socketfd, MutiRespsone);
}
void Room_Manager::OnChangelimit(const int socket_fd, const Header header, const char *content)
{
    Room_Protobuf::ChangePlayerlimit_Request Request;
    Request.ParseFromArray(content, header.length);

    int limit = Request.limit();
    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);
    if (!user)
        return;

    vector<User_Info *> Remain_Player_Userinfo;

    if (false == Changelimit(user, limit, Remain_Player_Userinfo))
        return;

    Room_Protobuf::ChangePlayerlimit_MutiResponse MutiRespsone;
    MutiRespsone.set_limit(limit);
    vector<int> Vec_socketfd;
    for (auto fuser : Remain_Player_Userinfo)
        Vec_socketfd.push_back(fuser->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socketfd, MutiRespsone);
}
void Room_Manager::OnChangeReady(const int socket_fd, const Header header, const char *content)
{
    Room_Protobuf::ChangeReady_Request Request;
    Request.ParseFromArray(content, header.length);

    Ready_State state = (Ready_State)Request.state();
    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);
    if (!user)
        return;

    vector<User_Info *> Remain_Player_Userinfo;

    Ready_State New_state;
    if (false == ChangeReady(user, New_state, Remain_Player_Userinfo))
        return;

    Room_Protobuf::ChangeReady_MutiResponse MutiRespsone;
    MutiRespsone.set_name(user->Get_UserName());
    MutiRespsone.set_state((int)New_state);

    vector<int> Vec_socketfd;
    for (auto fuser : Remain_Player_Userinfo)
        Vec_socketfd.push_back(fuser->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socketfd, MutiRespsone);
}
void Room_Manager::OnChangeTeam(const int socket_fd, const Header header, const char *content)
{
    Room_Protobuf::ChangeTeam_Request Request;
    Request.ParseFromArray(content, header.length);

    Groups group = (Groups)Request.group();
    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);
    if (!user)
        return;

    vector<User_Info *> Remain_Player_Userinfo;
    Groups New_group;
    if (false == ChangeTeam(user, New_group, Remain_Player_Userinfo))
        return;

    Room_Protobuf::ChangeTeam_MutiResponse MutiRespsone;
    MutiRespsone.set_name(user->Get_UserName());
    MutiRespsone.set_group((int)New_group);

    vector<int> Vec_socketfd;
    for (auto fuser : Remain_Player_Userinfo)
        Vec_socketfd.push_back(fuser->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socketfd, MutiRespsone);
}

void Room_Manager::OnSendRoomMessage(const int socket_fd, const Header header, const char *content)
{
    Room_Protobuf::RoomMessage_Request Request;
    Request.ParseFromArray(content, header.length);

    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);
    if (!user)
        return;

    vector<PlayerInfo> playerinfo;
    if (!Get_RoomPlayer(user->Get_ID(), playerinfo))
        return;

    Room_Protobuf::RoomMessage_MutiResponse MutiResponse;
    MutiResponse.set_name(user->Get_UserName());
    MutiResponse.set_content(Request.content());

    vector<int> Vec_socket;
    for (auto &fplayer : playerinfo)
        Vec_socket.push_back(fplayer.user->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socket, MutiResponse);
    LOGINFO("Room_Manager::OnSendRoomMessage user sendroommessage : {} content : {}", user->Get_UserName(), Request.content());
}

void Room_Manager::OnRoomInfo(const int socket_fd, const Header header, const char *content)
{
    User_Info *user = Center_Server::Instance()->Get_Userinfo(socket_fd);

    Room_Protobuf::RoomInfo_Response Response;
    Room_Info *room;
    if (RoomInfo(user, room))
    {
        Room_Protobuf::RoomAllInfo *roominfo = new Room_Protobuf::RoomAllInfo();
        roominfo->set_room_id(room->room_id);
        roominfo->set_host_name(room->host_name);
        roominfo->set_playersnum(room->playersnum);
        roominfo->set_playerslimit(room->playerslimit);
        roominfo->set_mapid((int)room->mapid);
        Response.set_allocated_roominfo(roominfo);

        for (auto player : room->Get_player())
        {
            Room_Protobuf::Playerinfo *proto_playerinfo = roominfo->add_playerinfo();
            proto_playerinfo->set_playersname(player.user->Get_UserName());
            proto_playerinfo->set_state((int)player.state);
            proto_playerinfo->set_group((int)player.group);
        }
        user->states = USER_STATE::Room;
    }
    Center_Server::Instance()->SendTo_SendQueue(socket_fd, Response);
}

void Room_Manager::OnStartGame(const int socket_fd, const Header header, const char *content)
{
    User_Info *userinfo = Center_Server::Instance()->Get_Userinfo(socket_fd);
    vector<User_Info *> Remain_Player_Userinfo;

    string DS_IP;
    StartGame_Result result = StartGame(userinfo, Remain_Player_Userinfo, DS_IP);
    if (StartGame_Result::InnerError == result)
        return;

    Room_Protobuf::StartGame_MutiResponse MutiRespsone;
    MutiRespsone.set_result((int)result);
    if (StartGame_Result::Success == result)
    {
        userinfo->states = USER_STATE::Gaming;
        MutiRespsone.set_ds_ip(DS_IP);
    }

    vector<int> Vec_socketfd;
    for (auto fuser : Remain_Player_Userinfo)
        Vec_socketfd.push_back(fuser->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socketfd, MutiRespsone);
}

shared_ptr<Room_Protobuf::SerachRoom_Response> Room_Manager::Get_RoomList_ProtobufInfo()
{
    if (!RoomChanged)
        return Probuf_SharePtr;

    shared_ptr<Room_Protobuf::SerachRoom_Response> NewProbuf_SharePtr = make_shared<Room_Protobuf::SerachRoom_Response>();
    Room_Protobuf::SerachRoom_Response *ProtobufInfo = NewProbuf_SharePtr.get();
    for (auto &room : Room_list)
    {
        Room_Protobuf::RoomDisplayInfo *roominfo = ProtobufInfo->add_roominfo();
        roominfo->set_room_id(room->room_id);
        roominfo->set_host_name(room->host_name);
        roominfo->set_mapid((int)room->mapid);
        roominfo->set_playersnum(room->playersnum);
        roominfo->set_playerslimit(room->playerslimit);
    }

    Probuf_SharePtr = NewProbuf_SharePtr;
    RoomChanged = false;
    return Probuf_SharePtr;
}

CreateRoom_Result Room_Manager::CreateRoom(User_Info *user, const int playerslimit, const Map mapid)
{
    Check(user);
    Room_Info *room = new Room_Info();
    room->room_id = ++roomid_count;
    room->host_id = user->Get_ID();
    room->host_name = user->Get_UserName();
    room->playerslimit = playerslimit;
    room->mapid = mapid;
    room->Add_player(user);
    Room_list.emplace_back(room);
    Quick_map[user->Get_ID()] = room;
    RoomChanged = true;
    LOGINFO("Room_Manager::CreateRoom user createroom : {}, roomid : {}", user->Get_UserName(), room->room_id);
    return CreateRoom_Result::Success;
}

JoinRoom_Result Room_Manager::JoinRoom(User_Info *user, const int room_id, Room_Info *&Info)
{
    Check(user);
    for (auto &room : Room_list)
    {
        if (room->room_id == room_id)
        {
            if (room->playersnum < room->playerslimit)
            {
                room->Add_player(user);
                Quick_map[user->Get_ID()] = room;
                RoomChanged = true;
                Info = room;
                LOGINFO("Room_Manager::JoinRoom user joinroom : {}, roomid : {}, result : {}", user->Get_UserName(), room_id, "Success");
                return JoinRoom_Result::Success;
            }
            else
            {
                LOGINFO("Room_Manager::JoinRoom user joinroom : {}, roomid : {}, result : {}", user->Get_UserName(), room_id, "PlayerLimit");
                return JoinRoom_Result::PlayerLimit;
            }
        }
    }

    LOGINFO("Room_Manager::JoinRoom user joinroom : {}, roomid : {}, result : {}", user->Get_UserName(), room_id, "RoomNotFound");
    return JoinRoom_Result::RoomNotFound;
}

ExitRoom_Result Room_Manager::ExitRoom(User_Info *user, vector<User_Info *> &remain_player_userinfo, Room_Info *&room_out)
{
    if (Quick_map.find(user->Get_ID()) != Quick_map.end())
    {
        Room_Info *room = Quick_map[user->Get_ID()];

        if (!(room->Exit_player(user->Get_ID())))
            return ExitRoom_Result::Fail;
        Quick_map.erase(Quick_map.find(user->Get_ID()));

        RoomChanged = true;
        for (auto fuser : room->Get_player_Userinfo())
            remain_player_userinfo.push_back(fuser);

        LOGINFO("Room_Manager::ExitRoom user exitroom : {}, roomid : {}", user->Get_UserName(), room->room_id);
        if (room->playersnum <= 0)
            RemoveRoom(room);

        else if (room->host_id == user->Get_ID()) // need changehost
        {
            room->Changehost(room->playerinfo.front().user->Get_ID());
            room_out = room;
            return ExitRoom_Result::SWitch_Host;
        }

        return ExitRoom_Result::Success;
    }

    return ExitRoom_Result::Fail;
}

KickRoom_Result Room_Manager::KickRoom(User_Info *user, int kickid, vector<User_Info *> &remain_player_userinfo)
{
    if (Quick_map.find(user->Get_ID()) == Quick_map.end())
        return KickRoom_Result::Fail;

    Room_Info *room = Quick_map[user->Get_ID()];
    if (room->host_id != user->Get_ID())
        return KickRoom_Result::Fail;

    if (!(room->Exit_player(kickid)))
        return KickRoom_Result::Fail;

    Quick_map.erase(Quick_map.find(kickid));
    RoomChanged = true;
    for (auto &fuser : room->Get_player_Userinfo())
        remain_player_userinfo.push_back(fuser);
    return KickRoom_Result::Success;
}

StartGame_Result Room_Manager::StartGame(User_Info *user, vector<User_Info *> &remain_player_userinfo, string &DS_IP)
{
    if (Quick_map.find(user->Get_ID()) == Quick_map.end())
        return StartGame_Result::InnerError;

    Room_Info *room = Quick_map[user->Get_ID()];
    if (room->host_id != user->Get_ID())
        return StartGame_Result::InnerError;

    room->Lock();

    // Request New Game
    Game_Protobuf::Gameinfo *proto_room = new Game_Protobuf::Gameinfo();
    proto_room->set_hostid(room->host_id);
    proto_room->set_roomid(room->room_id);
    proto_room->set_mapid((int)room->mapid);
    proto_room->set_playersnum(room->playersnum);
    proto_room->set_playerslimit(room->playerslimit);
    for (auto &fplayer : room->playerinfo)
    {
        Game_Protobuf::PlayerAllinfo *proto_player = proto_room->add_playersallinfo();
        proto_player->set_id(fplayer.user->Get_ID());
        proto_player->set_token(fplayer.user->token);
        proto_player->set_playersname(fplayer.user->Get_UserName());
        proto_player->set_group((int)fplayer.group);
    }
    Game_Protobuf::NewGame_Request Request;
    Request.set_allocated_gameinfo(proto_room);
    Game_Protobuf::NewGame_Response Response;
    if (!RPC_Client::Instance()->Excute_RPCRequest(Request, Response, RPC_GameServer))
        return StartGame_Result::InnerError;
    NewGame_Result result = (NewGame_Result)Response.result();
    string DSIP = Response.ds_ip();
    if (Response.result() == (int)NewGame_Result::Fail)
        return StartGame_Result::InnerError;

    DS_IP = Response.ds_ip();
    RoomChanged = true;
    for (auto fuser : room->Get_player_Userinfo())
        remain_player_userinfo.push_back(fuser);

    LOGINFO("Room_Manager::StartGame : roomid :{} ", room->room_id);
    return StartGame_Result::Success;
}

bool Room_Manager::ChangeReadyState(User_Info *user, Ready_State state_in, vector<User_Info *> &remain_player_userinfo)
{
    if (Quick_map.find(user->Get_ID()) == Quick_map.end())
        return false;

    Room_Info *room = Quick_map[user->Get_ID()];

    if (!(room->ChangeReadyState(user->Get_ID(), state_in)))
        return false;

    for (auto &fuser : room->Get_player_Userinfo())
        remain_player_userinfo.push_back(fuser);

    return true;
}

bool Room_Manager::ChangeMap(User_Info *user, Map mapid, vector<User_Info *> &remain_player_userinfo)
{
    if (Quick_map.find(user->Get_ID()) == Quick_map.end())
        return false;

    Room_Info *room = Quick_map[user->Get_ID()];
    Map oldmapid = room->mapid;
    if (room->host_id != user->Get_ID())
        return false;

    if (!(room->ChangeMap(mapid)))
        return false;

    RoomChanged = true;
    for (auto fuser : room->Get_player_Userinfo())
        remain_player_userinfo.push_back(fuser);

    LOGINFO("Room_Manager::ChangeMap : roomid :{} ,oldmapid : {}, newmapid : {}", room->room_id, (int)oldmapid, (int)room->mapid);
    return true;
}

bool Room_Manager::Changelimit(User_Info *user, int limit, vector<User_Info *> &remain_player_userinfo)
{
    if (Quick_map.find(user->Get_ID()) == Quick_map.end())
        return false;

    Room_Info *room = Quick_map[user->Get_ID()];
    int oldlimit = room->playerslimit;
    if (room->host_id != user->Get_ID())
        return false;

    if (!(room->Changelimit(limit)))
        return false;

    RoomChanged = true;
    for (auto &fuser : room->Get_player_Userinfo())
        remain_player_userinfo.push_back(fuser);

    LOGINFO("Room_Manager::Changelimit : roomid :{} ,oldlimit : {}, newlimit : {}", room->room_id, oldlimit, room->playerslimit);
    return true;
}

bool Room_Manager::ChangeReady(User_Info *user, Ready_State &State_out, vector<User_Info *> &remain_player_userinfo)
{
    if (Quick_map.find(user->Get_ID()) == Quick_map.end())
        return false;

    Room_Info *room = Quick_map[user->Get_ID()];
    if (!(room->ChangeReadyState(user->Get_ID(), State_out)))
        return false;

    for (auto &fuser : room->Get_player_Userinfo())
        remain_player_userinfo.push_back(fuser);

    return true;
}

bool Room_Manager::ChangeTeam(User_Info *user, Groups &group_out, vector<User_Info *> &remain_player_userinfo)
{
    if (Quick_map.find(user->Get_ID()) == Quick_map.end())
        return false;

    Room_Info *room = Quick_map[user->Get_ID()];
    if (!(room->ChangeTeam(user->Get_ID(), group_out)))
        return false;

    for (auto &fuser : room->Get_player_Userinfo())
        remain_player_userinfo.push_back(fuser);

    return true;
}

bool Room_Manager::RoomInfo(User_Info *userinfo, Room_Info *&Info)
{
    if (Quick_map.find(userinfo->Get_ID()) == Quick_map.end())
        return false;

    Room_Info *room = Quick_map[userinfo->Get_ID()];
    if (room)
    {
        Info = room;
        return true;
    }
    return false;
}

bool Room_Manager::Get_RoomPlayer(int user_id, vector<PlayerInfo> &playerinfo)
{
    if (Quick_map.find(user_id) == Quick_map.end())
        return false;

    Room_Info *room = Quick_map[user_id];
    playerinfo = room->Get_player();

    return true;
}

bool Room_Manager::RemoveRoom(Room_Info *room)
{
    int result = false;
    for (auto it = Room_list.begin(); it != Room_list.end(); it++)
    {
        if ((*it) == room)
        {
            Room_list.erase(it);
            result = true;
            break;
        }
    }
    if (!result)
        return false;

    for (auto it = Quick_map.begin(); it != Quick_map.end(); it++)
    {
        if ((*it).second == room)
            Quick_map.erase(it);
    }
    LOGINFO("Room_Manager::RemoveRoom removeroom roomid : {}", room->room_id);
    delete room;
    RoomChanged = true;
    return true;
}

bool Room_Manager::RemoveUser(int fd)
{
    User_Info *user = Center_Server::Instance()->Get_Userinfo(fd);
    if (!user)
        return false;

    user->states = USER_STATE::Hall;

    vector<User_Info *> Remain_Player_Userinfo;
    Room_Info *room;
    ExitRoom_Result result = ExitRoom(user, Remain_Player_Userinfo, room);

    if (Remain_Player_Userinfo.size() <= 0)
        return true;

    Room_Protobuf::Playerinfo *proto_player = new Room_Protobuf::Playerinfo();
    proto_player->set_playersname(user->Get_UserName());

    Room_Protobuf::ExitRoom_MutiResponse MutiRespsone;
    MutiRespsone.set_allocated_playerinfo(proto_player);

    if (result == ExitRoom_Result::SWitch_Host)
    {
        MutiRespsone.set_changehost(1);
        MutiRespsone.set_new_hostname(room->host_name);
    }
    else
        MutiRespsone.set_changehost(0);

    vector<int> Vec_socketfd;
    for (auto fuser : Remain_Player_Userinfo)
        Vec_socketfd.push_back(fuser->Get_SocketFd());

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socketfd, MutiRespsone);
    return true;
}

bool Room_Manager::Check(User_Info *user)
{
    if (Quick_map.find(user->Get_ID()) == Quick_map.end())
        return true;
    RemoveUser(user->Get_SocketFd());
    return false;
}