#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include<unordered_map>
#include<iostream>
#include<functional>
#include<muduo/net/TcpConnection.h>
#include"../../thirdparty/json.hpp"
#include "model/usermodel.hpp"
#include "model/offlinemessagemodel.hpp"
#include "model/friendmodel.hpp"
#include "model/groupmodel.hpp"
#include "model/group.hpp"
#include "model/groupuser.hpp"
#include "Redis.hpp"
#include<mutex>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;
using msghandle = std::function<void(const TcpConnectionPtr&, json&, Timestamp)>;

class chatservice{
public:
    static chatservice* Intance();
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void regist(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void onechat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void  add_friend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void creategroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void add_group(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void group_chat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void login_out(const TcpConnectionPtr &conn, json &js, Timestamp time);
    void HandleRedisSubscribeMsg(int id, string message);
    void change_state();
    void clientcloseexcept(const TcpConnectionPtr &conn);
    void reset();
    msghandle gethandler(int Msgid);
private:
    chatservice();
    unordered_map<int, msghandle> mp;
    unordered_map<int, const TcpConnectionPtr> _connmp;
    UserModel model;
    OfflineMsgModel offlinemsgmodel;
    FriendModel friend_model;
    GroupModel group_model;
    mutex _connmutex;

    Redis redis;
};

#endif