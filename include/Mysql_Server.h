#pragma once

#include "header.h"
#include <mysql/mysql.h>

using namespace std;

struct MySQL_Query_Result
{
    MYSQL_RES Res;
};

class Mysql_Server
{
public:
    static Mysql_Server *Instance()
    {
        static Mysql_Server *m_Instance = new Mysql_Server();
        return m_Instance;
    }

public:
    bool CreateConnect();

    bool Select(const string &command, MYSQL_RES **result, int *result_count);
    bool Select(const string &&command, MYSQL_RES **result, int *result_count);
    bool Update(const string &command, int *affected_row_num);
    bool Update(const string &&command, int *affected_row_num);

private:
    Mysql_Server();
    ~Mysql_Server();

private:
    MYSQL *mysql;
};