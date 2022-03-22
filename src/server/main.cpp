#include "chatserver.hpp"
#include "chatservice.hpp"
#include<iostream>
#include<signal.h>


using namespace std;

void reset_handler(int)
{
    chatservice::Intance()->reset();
    exit(0);
}

int main(int argc, char *argv[])
{
    if(argc < 3)    return 0;
    signal(SIGINT, reset_handler);

    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "chatserver");

    server.start();
    while(1)
    {
        loop.loop();
    }

    return 0;
}