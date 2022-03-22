#ifndef FRIEND_H
#define FRIEND_H

#include<vector>
#include"user.hpp"

using namespace std;

class FriendModel{
public:
    void insert(int userid, int friendid);
    vector<User> query(int user_id);
};





#endif