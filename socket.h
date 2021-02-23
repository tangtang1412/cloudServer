//
// Created by 汤汤 on 2021/2/3.
//

#ifndef CLOUDSERVER_SOCKET_H
#define CLOUDSERVER_SOCKET_H

#endif //CLOUDSERVER_SOCKET_H

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


using namespace std;

class Socket{
public:
    Socket(int port);
    ~Socket();
private:
    int m_socketFd; //socket 返回
    int m_cfd; //accpet 返回
    struct sockaddr_in sockaddr_Clinet;
    struct sockaddr_in sockaddr_Server;
    char ipbuf[16];

    int keepAlive = 1; // 开启keepalive属性
    int keepIdle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测
    int keepInterval = 5; // 探测时发包的时间间隔为5 秒
    int keepCount = 3; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

public:
    int m_port;

public:
    void getClientInfo();
    bool myAccept();
    int& getSocketFd();
    int& getCfd();
};
