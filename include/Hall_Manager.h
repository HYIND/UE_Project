#pragma once

#include "header.h"

using namespace std;

class Hall_Manager
{
public:
    static Hall_Manager *Instance()
    {
        static Hall_Manager *m_Instance = new Hall_Manager();
        return m_Instance;
    }

public:
    void Push_User(User_Info *userinfo);
    int Process(Socket_Message *msg);

protected:
    void SendPublicMessage(const int socket_fd, const Header header, const char *content);

    template <typename T>
    int Get_Header_Type(T &message);

private:
    Hall_Manager(){};

};

