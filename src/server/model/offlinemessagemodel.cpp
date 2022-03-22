#include "../../../include/server/model/offlinemessagemodel.hpp"
#include "db.h"

void OfflineMsgModel::insert(int userid, string msg)
{
    char sql[100];
    sprintf(sql, "insert into OfflineMessage values(%d, '%s');", userid, msg.c_str());

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

void OfflineMsgModel::remove(int userid)
{
    char sql[100];
    sprintf(sql, "delete from OfflineMessage where userid = %d;", userid);

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

vector<string> OfflineMsgModel::query(int userid)
{
    char sql[100];
    sprintf(sql, "select message from OfflineMessage where userid = %d;", userid);

    vector<string> msg_array;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* result = mysql.query(sql);
        if(result != nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(result)) != nullptr)
            {
                msg_array.push_back(row[0]);
            }
            mysql_free_result(result);
        }
    }

    return msg_array;
}