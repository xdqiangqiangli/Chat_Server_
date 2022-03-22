#include"../../../include/server/model/usermodel.hpp"
#include<muduo/base/Logging.h>
#include<iostream>

using namespace std;

bool UserModel::insert(User& user)
{
    char sql[100];
    sprintf(sql, "insert into User(name, password,state) values('%s', '%s', '%s');", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            user.setId(mysql_insert_id(mysql.getconnection()));
            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    char sql[100];
    sprintf(sql, "select * from User where id = %d;",  id);

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
                user.setPwd(row[3]);
                mysql_free_result(result);
                return user;
            }
        }

    }

    return User();
}

void UserModel::change_state(User &user)
{
    char sql[100];
    sprintf(sql, "update User set state='%s' where id=%d;", user.getState().c_str(), user.getId());
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            LOG_INFO<<"updata state success";
        }
    }
}

void UserModel::reset()
{
    char sql[100];
    sprintf(sql, "update User set state= 'offline' where state='online';");
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            LOG_INFO<<"updata state success";
        }
    }
}