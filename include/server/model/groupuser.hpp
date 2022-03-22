#ifndef GROUPUSER_H
#define GROUPUSER_H

#include<string>
#include "user.hpp"

using namespace std;

class GroupUser:public User
{
public:
    void setrole(string rol) {this->role = rol;}
    string getrole(){return this->role;}
private:
    string role;
};



#endif