#ifndef USERMODEL_H
#define USERMODEL_H

#include"user.hpp"
#include"db.h"

class UserModel{
public:
    bool insert(User& user);
    User query(int id);
    void change_state(User &user);
    void reset();
};



#endif