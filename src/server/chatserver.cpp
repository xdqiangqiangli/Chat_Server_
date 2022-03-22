#include "chatserver.hpp"
#include <functional>
#include "../../thirdparty/json.hpp"
#include "chatservice.hpp"

using namespace  std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg):_server(loop, listenAddr, nameArg),_loop(loop)
{
    _server.setConnectionCallback(std::bind(&ChatServer::On_Connect, this, _1));

    _server.setMessageCallback(std::bind(&ChatServer::On_Message, this, _1, _2, _3));

    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::On_Connect(const muduo::net::TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        chatservice::Intance()->clientcloseexcept(conn);
        conn->shutdown();
    }
}
void ChatServer::On_Message(const TcpConnectionPtr& conn,
                            Buffer* buffer,
                            Timestamp time)
{
    json js = json::parse(buffer->retrieveAllAsString());
    auto msghandler = chatservice::Intance()->gethandler(js["msgid"].get<int>());
    msghandler(conn, js, time);
}


