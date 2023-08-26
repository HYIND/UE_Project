#include "Center_Server.h"

User_Info *Center_Server::Get_Userinfo(int socket_fd)
{
    for (auto v : user_list)
    {
        if (v->sockinfo->tcp_fd == socket_fd)
            return v;
    }
    return nullptr;
}

int Center_Server::OnProcess(Socket_Message *msg)
{
    const Header &header = msg->header;
    const int socket_fd = msg->socket_fd;
    char *content = msg->content;

    switch (header.type)
    {
    case Request_Logout:
    {
        OnLogout(socket_fd, header, content);
    }
    case ping_Request:
    {
        // OnPing(socket_fd, header, content);
        break;
    }

    // Room
    case Request_CreateRoom:
    case Request_SerachRoom:
    case Request_JoinRoom:
    case Request_ExitRoom:
    case Request_KickRoom:
    case Request_SendRoomMessage:
    case Request_ChangeMap:
    case Request_Changelimit:
    case Request_ChangeReadyState:
    case Request_StartGame:
    case Request_ChangeTeam:
    {
        Room_Manager::Instance()->Process(msg);
        break;
    }

    // Hall
    case Request_SendPublicMessage:
    {
        Hall_Manager::Instance()->Process(msg);
        break;
    }
    }
    delete msg;
    msg = nullptr;

    return 1;
}

int Center_Server::OnProcess(Token_SocketMessage *TokenMsg)
{
    if (!CheckToken(TokenMsg->token))
        return 0;
    Socket_Message *msg = TokenMsg->msg;
    const Header &header = msg->header;
    const int socket_fd = msg->socket_fd;
    char *content = msg->content;

    bool result = false;
    switch (header.type)
    {
    case Request_Logout:
    {
        OnLogout(socket_fd, header, content);
        result = true;
        break;
    }
    case ping_Request:
    {
        // OnPing(socket_fd, header, content);
        result = true;
        break;
    }

    // Room
    case Request_CreateRoom:
    case Request_SerachRoom:
    case Request_JoinRoom:
    case Request_ExitRoom:
    case Request_KickRoom:
    case Request_SendRoomMessage:
    case Request_ChangeMap:
    case Request_Changelimit:
    case Request_ChangeReadyState:
    case Request_StartGame:
    case Request_ChangeTeam:
    case Request_RoomInfo:
    {
        Room_Manager::Instance()->Process(msg);
        result = true;
        break;
    }

    // Hall
    case Request_SendPublicMessage:
    {
        Hall_Manager::Instance()->Process(msg);
        result = true;
        break;
    }
    }

    return result ? 1 : 0;
}

void Center_Server::Push_LoginUser(User_Info *userinfo)
{
    userinfo->states = USER_STATE::Hall;

    user_list.emplace_back(userinfo);
    Token_map[userinfo->Get_Token()] = userinfo;

    LOGINFO("Center_Server::Push_LoginUser user login : {} IP : {} Token : {}", userinfo->Get_UserName(), userinfo->Get_IP(), userinfo->token);
}

void Center_Server::OnLogout(const int socket_fd, const Header header, const char *content)
{
    User_Info *user = Get_Userinfo(socket_fd);
    if (!user)
        return;

    auto it = Token_map.find(user->Get_Token());
    if (it != Token_map.end())
        Token_map.erase(it);
    user->states = USER_STATE::Logining;

    // delfd(epoll, socket_fd);
    for (auto it = user_list.begin(); it != user_list.end(); it++)
    {
        if ((*it) == user)
        {
            LOGINFO("Center_Server::OnLogout user logout : {}", user->Get_UserName());
            user_list.erase(it);
            break;
        }
    }
}

// bool Center_Server::Check_Reconnect(Socket_Info *sockinfo, string &token)
// {
//     Login_Protobuf::Reconnect_Response Response;

//     auto map_it = Token_map.find(token);
//     if (map_it == Token_map.end())
//     {
//         Response.set_result((int)Reconnect_Result::Fail);
//         SendTo_SendQueue(sockinfo->Get_SocketFd(), Response);
//         return false;
//     }
//     Token_map.erase(map_it);

//     for (auto it = wait_list.begin(); it != wait_list.end(); it++)
//     {
//         HeartCount &waituser = *it;
//         if (waituser.userdata->Get_Token() == token)
//         {
//             wait_list.erase(it);

//             User_Info *user = waituser.userdata;
//             user->SetSocketinfo_Move(sockinfo);
//             user->states = USER_STATE::Hall;
//             user->token = Get_Token();

//             user_list.emplace_back(user);
//             addfd(epoll, sockinfo->Get_SocketFd());
//             HeartCount heartcount(user);
//             HeartBeat_map.insert(pair<int, HeartCount>(user->Get_SocketFd(), heartcount));
//             Token_map[user->Get_Token()] = user;

//             Response.set_result((int)Reconnect_Result::Success);
//             Response.set_token(user->token);

//             SendTo_SendQueue(user->Get_SocketFd(), Response);

//             return true;
//         }
//     }
//     Response.set_result((int)Reconnect_Result::Fail);
//     SendTo_SendQueue(sockinfo->Get_SocketFd(), Response);
//     return false;
// }

bool Center_Server::CheckToken(string &token)
{
    auto map_it = Token_map.find(token);
    if (map_it == Token_map.end())
    {
        LOGERROR("Center_Server::CheckToken ErrorToken : {} ", token);
        return false;
    }
    return true;
}

void Center_Server::RemoveUser(int fd)
{
    try
    {
        Room_Manager::Instance()->RemoveUser(fd);
        for (auto it = user_list.begin(); it != user_list.end(); it++)
        {
            if (((*it)->Get_SocketFd()) == fd)
            {
                user_list.erase(it);
                break;
            }
            LOGINFO("Center_Server::RemoveUser , user remove : {}", (*it)->Get_UserName());
        }
        for (auto it = Token_map.begin(); it != Token_map.end(); it++)
        {
            if (((*it).second->Get_SocketFd()) == fd)
            {
                Token_map.erase(it);
                break;
            }
        }
        // auto it = HeartBeat_map.find(fd);
        // if (it != HeartBeat_map.end())
        //     HeartBeat_map.erase(it);
    }
    catch (exception &e)
    {
        perror(e.what());
        LOGINFO("Center_Server::RemoveUser , an unknown error :{}", e.what());
    }
}
