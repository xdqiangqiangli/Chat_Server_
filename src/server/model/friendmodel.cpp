#include "../../../include/server/model/friendmodel.hpp"
#include "db.h"

void FriendModel::insert(int userid, int friendid)
{    
    char sql[100];
    sprintf(sql, "insert into Friend values(%d, %d);", userid, friendid);
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

vector<User> FriendModel::query(int user_id)
{
    char sql[100];
    sprintf(sql, "select  u.id, u.name, u.state from User u inner join Friend f on u.id = f.friendid where f.userid = %d;",  user_id);

    vector<User> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* result = mysql.query(sql);
        if(result != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            if(row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(result);
        }

    }
    return vec;
}
