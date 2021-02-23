//
// Created by 汤汤 on 2021/2/3.
//

#include "socket.h"


Socket::Socket(int port){

    ipbuf[16] = { 0 };
    m_port = port;
    m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socketFd == -1)
    {
        cout << "socket error" << endl;
        throw "socket";
    }
    memset(&sockaddr_Server, 0, sizeof(sockaddr_Server));
    sockaddr_Server.sin_family = AF_INET;
    sockaddr_Server.sin_port = htons(m_port);
    sockaddr_Server.sin_addr.s_addr = INADDR_ANY;
    int ret;
    ret = ::bind(m_socketFd, (struct sockaddr*)&sockaddr_Server, sizeof(sockaddr_Server));
    if(ret == -1)
    {
        cout << "bind error" << endl;
        throw "bind";
    }
    int opt = 1;
    ret = setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(m_socketFd));
    if(ret == -1)
    {
        cout << "setsockopt error" << endl;
        throw "setsockopt";
    }
    ret = listen(m_socketFd, 128);
    if(ret == -1)
    {
        cout << "listen error" << endl;
        throw "listen";
    }
}

Socket::~Socket() {
    //close(m_cfd);
//    close(m_socketFd);
}

int& Socket::getSocketFd() {
    return m_socketFd;
}


int& Socket::getCfd() {
    return m_cfd;
}

void Socket::getClientInfo() {
    cout <<"客户端地址：" <<inet_ntop(AF_INET, &sockaddr_Clinet.sin_addr.s_addr,ipbuf,sizeof(ipbuf));
    cout << "客户端端口：" << ntohs(sockaddr_Clinet.sin_port) << endl;
}

bool Socket::myAccept() {
    memset(&sockaddr_Clinet, 0, sizeof(sockaddr_Clinet));
    int clientLength = sizeof(sockaddr_Clinet);
    m_cfd = ::accept(m_socketFd, (struct sockaddr*)&sockaddr_Clinet,(socklen_t *)&clientLength);


    setsockopt(m_cfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
//    setsockopt(m_cfd, IPPROTO_TCP, TCP_KEEPALIVE, (void*)&keepIdle, sizeof(keepIdle));
//    setsockopt(m_cfd, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval));
//    setsockopt(m_cfd, IPPROTO_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount));
    if(m_cfd == -1)
    {
        if(errno == EINTR || errno == EBADF)
        {
            cout << "EINTR|EBADF" << errno << endl;
            return false;
        }
        cout << errno << endl;
        cout << "accept error " << endl;
        throw "accept";
    }
    return true;
}
