#include "Mysql_Server.h"

Mysql_Server::Mysql_Server()
{
    mysql = mysql_init(NULL);
}

Mysql_Server::~Mysql_Server()
{
    if (!mysql)
    {
        mysql_close(mysql);
    }
}

bool Mysql_Server::CreateConnect()
{
    string host = Config::Instance()->Read<string>("mysql_host", "127.0.0.1");
    int port = Config::Instance()->Read("mysql_port", 0);
    string database = Config::Instance()->Read<string>("mysql_database", "NULL");
    string user = Config::Instance()->Read<string>("mysql_user", "NULL");
    string password = Config::Instance()->Read<string>("mysql_password", "NULL");

    // LOGINFO("host: {},port: {},database: {}, user: {}, password: {}", host, port, database, user, password);

    if (port == 0)
        return false;

    if (!mysql_real_connect(this->mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, nullptr, 0))
    {
        return false;
        LOGINFO("Connect Database Success!");
    }

    LOGINFO("Connect Database Success! host: {},port: {},database: {}", host, port, database);
    return true;
}

bool Mysql_Server::Select(const string &command, MYSQL_RES **result, int *result_count)
{
    if (mysql_query(mysql, command.c_str()) != 0)
        return false;
    MYSQL_RES *Res = mysql_store_result(mysql);
    if (result)
        *result = Res;
    if (result_count)
        *result_count = mysql_num_rows(Res);

    return true;
}
bool Mysql_Server::Select(const string &&command, MYSQL_RES **result, int *result_count)
{
    if (mysql_query(mysql, command.c_str()) != 0)
        return false;
    MYSQL_RES *Res = mysql_store_result(mysql);
    if (result)
        *result = Res;
    if (result_count)
        *result_count = mysql_num_rows(Res);

    return true;
}

bool Mysql_Server::Update(const string &command, int *affected_row_num)
{
    if (mysql_query(mysql, command.c_str()) != 0)
        return false;

    if (affected_row_num)
        *affected_row_num = mysql_affected_rows(mysql);

    return true;
}

bool Mysql_Server::Update(const string &&command, int *affected_row_num)
{
    if (mysql_query(mysql, command.c_str()) != 0)
        return false;

    if (affected_row_num)
        *affected_row_num = mysql_affected_rows(mysql);
        
    return true;
}