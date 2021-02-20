//
// Created by 汤汤 on 2021/2/3.
//

#ifndef CLOUDSERVER_WORK_H
#define CLOUDSERVER_WORK_H

#endif //CLOUDSERVER_WORK_H
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "socket.h"
#include <vector>


using namespace std;

#define GETDIR 1
#define SENDFILE 2
#define RECFILE 3
#define M_CLOSE 9

#define SIZE 1024

bool working(Socket& socketfd,int flag);
bool sendFile(int& cfd);
bool recFile(int& cfd);
bool sendDirectory(int& cfd, vector<string>& fileName);
bool scanfDir(char *root, vector<string>& fileName);
