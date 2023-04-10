#pragma once

#include "../protobuf/Login_Protocol.pb.h"
#include "../protobuf/Hall_Protocol.pb.h"
#include "../protobuf/Room_Protocol.pb.h"
#include "../protobuf/Game_Protocol.pb.h"

enum Connect
{
    Heart_Package = 404,

    Request_Reconnect = 500,
    Response_Reconnect = 505
};

enum Center_Server_MsgNum
{
    // Centre
    ping_Request = 600,
    Request_Loginout = 720,

    ping_Response = 605,
    Response_Loginout = 725,
};

enum Login_Server_MsgNum
{
    Request_Login = 700,
    Request_Signup = 710,

    Response_Login = 705,
    Response_Signup = 715
};

enum Hall_Server_MsgNum
{
    Request_SendPublicMessage = 400,

    MutiResponse_SendPublicMessage = 500,
};

enum Room_Server_MsgNum
{
    Request_CreateRoom = 800,
    Request_SerachRoom = 810,
    Request_JoinRoom = 820,
    Request_ExitRoom = 830,
    Request_KickRoom = 840,
    Request_ChangeMap = 850,
    Request_Changelimit = 860,
    Request_ChangeReadyState = 870,
    Request_SendRoomMessage = 880,
    Request_ChangeTeam = 890,

    Response_CreateRoom = 900,
    Response_SerachRoom = 910,
    Response_JoinRoom = 920,
    MutiResponse_JoinRoom = 925,
    Response_ExitRoom = 930,
    MutiResponse_ExitRoom = 935,
    Response_KickRoom = 940,
    MutiResponse_ChangeMap = 950,
    MutiResponse_Changelimit = 960,
    MutiResponse_ChangeReadyState = 970,
    MutiResponse_SendRoomMessage = 980,
    MutiResponse_ChangeTeam = 990,

    Request_StartGame = 1800,
    MutiResponse_StartGame = 2000

};

enum Game_Server_MsgNum
{
    Request_NewGame = 4000,
    Response_NewGame = 5000
};

template <typename T>
int Get_Header_Type(T &message)
{
    if (is_same<T, Login_Protobuf::Logout_Response>::value)
        return Center_Server_MsgNum::Response_Loginout;
    if (is_same<T, Hall_Protobuf::Ping_Response>::value)
        return Center_Server_MsgNum::ping_Response;
    if (is_same<T, Hall_Protobuf::PublicMessage_MutiResponse>::value)
        return Hall_Server_MsgNum::MutiResponse_SendPublicMessage;
    if (is_same<T, Room_Protobuf::RoomMessage_MutiResponse>::value)
        return Room_Server_MsgNum::MutiResponse_SendRoomMessage;
    if (is_same<T, Room_Protobuf::SerachRoom_Response>::value)
        return Room_Server_MsgNum::Response_SerachRoom;
    if (is_same<T, Room_Protobuf::JoinRoom_Response>::value)
        return Room_Server_MsgNum::Response_JoinRoom;
    if (is_same<T, Room_Protobuf::JoinRoom_MutiResponse>::value)
        return Room_Server_MsgNum::MutiResponse_JoinRoom;
    if (is_same<T, Room_Protobuf::ExitRoom_Response>::value)
        return Room_Server_MsgNum::Response_ExitRoom;
    if (is_same<T, Room_Protobuf::ExitRoom_MutiResponse>::value)
        return Room_Server_MsgNum::MutiResponse_ExitRoom;
    if (is_same<T, Room_Protobuf::KickRoom_Response>::value)
        return Room_Server_MsgNum::Response_KickRoom;
    if (is_same<T, Room_Protobuf::ChangeMap_MutiResponse>::value)
        return Room_Server_MsgNum::MutiResponse_ChangeMap;
    if (is_same<T, Room_Protobuf::ChangePlayerlimit_MutiResponse>::value)
        return Room_Server_MsgNum::MutiResponse_Changelimit;
    if (is_same<T, Room_Protobuf::ChangeReady_MutiResponse>::value)
        return Room_Server_MsgNum::MutiResponse_ChangeReadyState;
    if (is_same<T, Room_Protobuf::ChangeTeam_MutiResponse>::value)
        return Room_Server_MsgNum::MutiResponse_ChangeTeam;
    if (is_same<T, Room_Protobuf::CreateRoom_Response>::value)
        return Room_Server_MsgNum::Response_CreateRoom;

    if (is_same<T, Room_Protobuf::StartGame_Request>::value)
        return Room_Server_MsgNum::Request_StartGame;
    if (is_same<T, Room_Protobuf::StartGame_MutiResponse>::value)
        return Room_Server_MsgNum::MutiResponse_StartGame;

    if (is_same<T, Game_Protobuf::NewGame_Request>::value)
        return Game_Server_MsgNum::Request_NewGame;
    if (is_same<T, Game_Protobuf::NewGame_Response>::value)
        return Game_Server_MsgNum::Response_NewGame;

    if (is_same<T, Login_Protobuf::Heart_Package>::value)
        return Connect::Heart_Package;
    if (is_same<T, Login_Protobuf::Reconnect_Request>::value)
        return Connect::Response_Reconnect;

    return 0;
}
