#ifndef  CHATSERVER_H_
#define CHATSERVER_H_

#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "chatservice.hpp"
using namespace muduo;
using namespace muduo::net;

class ChatServer{
public:
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    void start();
private:
    void On_Connect(const muduo::net::TcpConnectionPtr &conn);
    void On_Message(const TcpConnectionPtr&, Buffer* buffer, Timestamp time);

    muduo::net::TcpServer _server;
    muduo::net::EventLoop *_loop;
};




#endif