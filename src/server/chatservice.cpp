#include"chatservice.hpp"
#include"public.hpp"
#include"../../server/model/usermodel.hpp"
#include "../../server/model/offlinemessagemodel.hpp"
#include"../../server/model/user.hpp"
#include "../../thirdparty/json.hpp"
#include "../../server/model/groupmodel.hpp"
#include "../../server/model/friendmodel.hpp"
#include<iostream>
#include<vector>
#include<string>
#include<mutex>
#include<muduo/base/Logging.h>

using namespace std;
using namespace muduo;
using json = nlohmann::json;


chatservice::chatservice()
{
    mp.insert({login_msg, std::bind(&chatservice::login, this, _1, _2, _3)});
    mp.insert({regist_msg, std::bind(&chatservice::regist, this, _1, _2, _3)});
    mp.insert({one_chat_msg, std::bind(&chatservice::onechat, this, _1, _2, _3)});
    mp.insert({add_friend_msg, std::bind(&chatservice::add_friend, this, _1, _2, _3)});
    mp.insert({create_group_msg, std::bind(&chatservice::creategroup, this, _1, _2, _3)});
    mp.insert({add_group_msg, std::bind(&chatservice::add_group, this, _1, _2, _3)});
    mp.insert({group_chat_msg, std::bind(&chatservice::group_chat, this, _1, _2, _3)});
    mp.insert({login_out_msg, std::bind(&chatservice::login_out, this, _1, _2, _3)});

    if(redis.connect())
    {
        redis.init_notify_handler(std::bind(&chatservice::HandleRedisSubscribeMsg, this, _1, _2));
    }
}

msghandle chatservice::gethandler(int Msgid)
{
    auto t = chatservice::mp.find(Msgid);
    if(t == mp.end())
    {
        return [=](const TcpConnectionPtr&, json&, Timestamp){
            LOG_ERROR << "Msgid:" << Msgid << "can not find handler";
        } ;
    }
    return mp[Msgid];
}

chatservice* chatservice::Intance()
{
    static chatservice Instance;
    return &Instance;
}

void chatservice::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    string password = js["password"];
    User user = model.query(id);
    json respose;
    respose["msgid"] = login_msg_ACK;
    if(user.getId() == id && user.getPwd() == password)
    {
        if(user.getState() == "online")
        {
            respose["errno"] = 1;
            respose["errmsg"] = "user already online";
            LOG_ERROR << "user already online";
        }
        else
        {
            user.setState("online");
            model.change_state(user);

            redis.subscribe(id);

            {
                lock_guard<mutex> lk(_connmutex);
                _connmp.insert({id, conn});
            }

            vector<string> vec = offlinemsgmodel.query(id);
            if(!vec.empty())
            {
                respose["offlinemsg"] = vec;
                offlinemsgmodel.remove(id);
            }

            vector<User> vec2 = friend_model.query(id);
            if(!vec2.empty())
            {
                vector<string> friend_info;
                for(User &t:vec2)
                {
                    json tmp;
                    tmp["id"] = t.getId();
                    tmp["name"] = t.getName();
                    tmp["state"] = t.getState();
                    friend_info.push_back(tmp.dump());
                }
                respose["friends"] = friend_info;
            }

            vector<Group> vec3 = group_model.query(id);
            if(!vec3.empty())
            {
                vector<string>  group_tmp;
                for(auto &t:vec3)
                {
                    json tmp;
                    tmp["id"] = t.getId();
                    tmp["groupname"] = t.getName();
                    tmp["groupdesc"] = t.getDesc();
                    vector<string> g_user;
                    for(GroupUser &m:t.getGroupUser())
                    {
                        json u_tmp;
                        u_tmp["id"] = m.getId();
                        u_tmp["name"] = m.getName();
                        u_tmp["state"] = m.getState();
                        u_tmp["role"] = m.getrole();
                        g_user.push_back(u_tmp.dump());
                    }
                    tmp["users"] = g_user;
                    group_tmp.push_back(tmp.dump());
                }
                respose["groups"] = group_tmp;
            }


            respose["id"] = user.getId();
            respose["name"] = user.getName();
            respose["errno"] = 0;
        }
    }
    else
    {
        respose["errno"] = 1;
        respose["errmsg"] = "user id or password error";
        LOG_ERROR << "user id or password error";
    }
    conn->send(respose.dump());        
}

void chatservice::regist(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO << "do regist service!";
    string name = js["name"]; 
    string password = js["password"];
    User user;
    user.setName(name);
    user.setPwd(password);
    if(model.insert(user))
    {
        json respose;
        respose["msgid"] = regist_msg_ACK;
        respose["id"] = user.getId();
        respose["errno"] = 0;
        conn->send(respose.dump());
    }
    else
    {
        json respose;
        respose["msgid"] = regist_msg_ACK;
        respose["errno"] = 1;
        conn->send(respose.dump());
    }
}

void chatservice::onechat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"];

    {
        lock_guard<mutex> lk(_connmutex);
        auto tmp = _connmp.find(toid);
        if(tmp != _connmp.end())
        {
            tmp->second->send(js.dump());
            return;
        }
    }

    User user;
    user = model.query(toid);
    if(user.getState() == "online")
    {
        redis.publish(toid, js.dump());
        return;
    }

    offlinemsgmodel.insert(toid, js.dump());
}

void chatservice::creategroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int groupid = js["groupid"];
    int userid = js["id"];
    string groupname = js["groupname"];
    string groupdesc = js["groupdesc"];

    group_model.create_group(userid, groupid, groupname, groupdesc);
}

void chatservice::add_group(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int groupid = js["groupid"];
    int userid = js["id"];
    string grouprole = js["grouprole"];

    group_model.add_group(userid, groupid,grouprole);
}

void chatservice::group_chat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"];
    int groupid = js["groupid"];

    vector<int> vec = group_model.send_group_msg(id, groupid);

    for(int i=0; i<vec.size(); i++)
    {
        {
             lock_guard<mutex> lk(_connmutex);
            auto tmp = _connmp.find(vec[i]);
           if(tmp != _connmp.end())
            {
                tmp->second->send(js.dump());
                continue;
            }
        }

        User user;
        user = model.query(id);
        if(user.getState() == "online")
        {
            redis.publish(id, js.dump());
            continue;
        }

        offlinemsgmodel.insert(id, js.dump());
    }
}

void  chatservice::add_friend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    friend_model.insert(userid, friendid);
}

void chatservice::login_out(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"];
    
    User user;
    user.setId(id);
    user.setState("offline");

    {
        lock_guard<mutex> lk(_connmutex);
        _connmp.erase(_connmp.find(id));
    }
    redis.unsubscribe(id);

    model.change_state(user);
}

void chatservice::clientcloseexcept(const TcpConnectionPtr &conn)
{
    User user;

    {
        lock_guard<mutex> lk(_connmutex);
        for(auto it = _connmp.begin(); it != _connmp.end(); it++)
        {
            if(it->second == conn)
            {
                user.setId(it->first);
                user.setState("offline");
                redis.unsubscribe(it->first);
                _connmp.erase(it);
            }
        }
    }
    
    if(user.getId() != -1)
    {
        model.change_state(user);
    }
}

void chatservice::reset()
{
    {
        lock_guard<mutex> lk(_connmutex);
        for(auto it = _connmp.begin(); it != _connmp.end(); it++)
        {
            redis.unsubscribe(it->first);
        }
    }
    model.reset();
}

void chatservice::HandleRedisSubscribeMsg(int id, string message)
{
    if(_connmp.find(id) != _connmp.end())
    {
        _connmp[id]->send(message);
    }

    offlinemsgmodel.insert(id,message);
}