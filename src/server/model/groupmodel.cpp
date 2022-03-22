#include "../../../include/server/model/groupmodel.hpp"
#include "../../../include/server/model/group.hpp"
#include "../../../include/server/model/groupuser.hpp"
#include "db.h"

void GroupModel::create_group(int userid, int groupid, string groupname, string desc)
{
    char sql[100], sql2[100];
    sprintf(sql, "insert into AllGroup values(%d, '%s', '%s');", groupid, groupname.c_str(), desc.c_str());
    sprintf(sql2, "insert into GroupInfo values(%d, %d, 'create');", groupid, userid);
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
        mysql.update(sql2);
    }
}

void GroupModel::add_group(int userid, int groupid, string role)
{
    char sql[100];
    sprintf(sql, "insert into GroupInfo values(%d, %d, '%s');", groupid, userid,role.c_str());
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }    
}

vector<Group> GroupModel::query(int userid)
{
    char sql[1024];
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from AllGroup a inner join GroupInfo b on b.groupid=a.id where b.userid = %d;",  userid);

    vector<Group> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* result = mysql.query(sql);
        if(result != nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(result)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                vec.push_back(group);
            }
            mysql_free_result(result);
        }

        for(int i=0; i<vec.size(); i++)
        {
            sprintf(sql, "select a.id,a.name,a.state,b.grouprole from User a inner join GroupInfo b on a.id=b.userid where b.groupid = %d;",  vec[i].getId());
            MYSQL_RES* result = mysql.query(sql);
            if(result != nullptr)
            {
                MYSQL_ROW row;
                if((row=mysql_fetch_row(result)) != nullptr)
                {
                    GroupUser guser;
                    guser.setId(atoi(row[0]));
                    guser.setName(row[1]);
                    guser.setState(row[2]);
                    vec[i].setGroupUser(guser);
                }
                mysql_free_result(result);
            }
        }
    }
    return vec;
}

vector<int> GroupModel::send_group_msg(int userid, int groupid)
{
    char sql[100];
    sprintf(sql, "select a.id from User a inner join GroupInfo b on a.id=b.userid where b.groupid = %d;",  groupid);

    MySQL mysql;
    vector<int> vec;
    if(mysql.connect())
    {
        MYSQL_RES* result = mysql.query(sql);
        if(result != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(result);
            while(row != nullptr)
            {
                Group group;
                vec.push_back(atoi(row[0]));
            }
            mysql_free_result(result);
        }
    }
    return vec;
}
