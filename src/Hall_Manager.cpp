#include "Hall_Manager.h"
#include "Center_Server.h"
#include "MsgNum.h"


int Hall_Manager::Process(Socket_Message *msg)
{
    const Header &header = msg->header;
    const int socket_fd = msg->socket_fd;
    char *content = msg->content;

    switch (header.type)
    case Request_SendPublicMessage:
    {
        SendPublicMessage(socket_fd, header, content);
        break;
    }

    return 1;
}

void Hall_Manager::SendPublicMessage(const int socket_fd, const Header header, const char *content)
{
    Hall_Protobuf::PublicMessage_Request Request;
    Request.ParseFromArray(content, header.length);

    Hall_Protobuf::PublicMessage_MutiResponse MutiResponse;
    MutiResponse.set_name(Center_Server::Instance()->Get_Userinfo(socket_fd)->Get_UserName());
    MutiResponse.set_content(Request.content());

    vector<int> Vec_socket;
    for (auto userinfo : Center_Server::Instance()->Get_user_list())
    {
        if (userinfo->states == USER_STATE::Hall)
            Vec_socket.push_back(userinfo->Get_SocketFd());
    }

    Center_Server::Instance()->SendMutiTo_SendQueue(Vec_socket, MutiResponse);
    LOGINFO("Hall_Manager::SendPublicMessage user sendpublicmessage : {} content : {}", Center_Server::Instance()->Get_Userinfo(socket_fd)->Get_UserName(), Request.content());
}