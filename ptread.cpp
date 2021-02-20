//
// Created by 汤汤 on 2021/2/3.
//

#include "ptread.h"

//回收子进程
void recycle(int num) {
    while(true)
    {
        pid_t pid = waitpid(-1, nullptr, WNOHANG);
        if(pid > 0)
        {
            std::cout << "child process died, pid: " << pid << endl;
        }
        else
        {
            break;
        }
    }
}
