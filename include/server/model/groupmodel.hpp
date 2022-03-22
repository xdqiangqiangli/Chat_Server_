#ifndef GROUPMODEL_H
#define GROUPMODEL_H
#include<string>
#include"group.hpp"
#include<vector>
using namespace std;

class GroupModel
{
public:
    void create_group(int userid, int groupid, string groupname, string desc);
    void add_group(int userid, int groupid, string role = "normal");
    vector<Group> query(int userid);
    vector<int> send_group_msg(int userid, int groupid);
};

#endif