#ifndef GROUP_H
#define GROUP_H

#include<string>
#include<vector>
#include"groupuser.hpp"
using namespace std;

class Group{
public:
    void setId(int id) {this->id = id;}
    void setName(string name){this->name = name;}
    void setDesc(string desc){this->desc = desc;}
    void setGroupUser(const GroupUser user){this->vec.push_back(user);}

    int getId() {return this->id;}
    string getName() {return this->name;}
    string getDesc() {return this->desc;}
    vector<GroupUser> getGroupUser()    {return this->vec;}
private:
    int id;
    string name;
    string desc;
    vector<GroupUser> vec;
};


#endif