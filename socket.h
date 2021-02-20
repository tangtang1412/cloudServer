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
public:
    int m_port;

public:
    void getClientInfo();
    bool myAccept();
    int& getSocketFd();
    int& getCfd();
};
