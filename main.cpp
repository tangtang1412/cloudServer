#include <iostream>
#include "ptread.h"
#include "work.h"
#include <vector>

#define PORT 5555

int main() {
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = recycle;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, nullptr);
    Socket socketfd(PORT);
    int flag;
    char c_flag;
    try{
        while(true)
        {
            if(!socketfd.myAccept())
            {
                continue;
            }
            socketfd.getClientInfo();
            pid_t processId = fork();
            if(processId == 0)
            {
                cout << "child pid: " << getpid() << endl;
                close(socketfd.getSocketFd());
                while(true)
                {
                    int ret = read(socketfd.getCfd(),&c_flag,sizeof(char));
                    if(ret <= 0)
                    {
                        //cout << "ret : " << ret << endl;
                        break;
                    }
                    cout <<" flag : "<< c_flag << endl;
                    //printf("flag : %x\n", c_flag);
                    flag = atoi(&c_flag);
                    /* #define GETDIR 1 获取服务器目录
                        #define SENDFILE 2 传送文件
                        #define RECFILE 3  接受文件
                     */
                    if(flag == M_CLOSE)
                    {
                        break;
                    }
                    if(!working(socketfd, flag))
                    {
                        break;
                    }
                }
                close(socketfd.getCfd());
                exit(0);
            }
            else if(processId > 0)
            {
                cout << "father pid: " << getpid() << endl;
                cout << "pid: " << processId << endl;
                close(socketfd.getCfd());
            }
            else if(processId < 0)
            {
                cout << "fork error" << endl;
                break;
            }
        }
    }
    catch (...)
    {
        cout << "error" << endl;
    }
    cout << "程序结束" << endl;
    return 0;
}
