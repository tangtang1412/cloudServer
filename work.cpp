//
// Created by 汤汤 on 2021/2/3.
//

#include "work.h"
#include <vector>
#include <sys/stat.h>
#include <dirent.h>

bool working(Socket& socketfd, int flag) {
    vector<string> name;
    switch(flag)
    {
        //获取服务器文件目录
        case GETDIR:
            //recFile(socketfd.getCfd());
            scanfDir("./", name);
            sendDirectory(socketfd.getCfd(),name);
            break;
        //传送文件给Clint
        case SENDFILE:
            sendFile(socketfd.getCfd());
            break;
        //接受client文件
        case RECFILE:
            recFile(socketfd.getCfd());
            break;
        default :
            cout << "unknow" << endl;
            return false;
    }
    cout << "单次操作结束" << endl;
    return true;
}

bool sendFile(int& cfd) {
    string fileName;
    fstream inFile;
    char buf[SIZE] = { 0 };
    int ret = 0;
    int size = 0;
    //读取client 需要的文件名称
    size = read(cfd, buf, SIZE);
    cout << "read size :  " << size << endl;
    fileName = buf;
    cout << "fileName: "<< fileName << endl;
    //cout << "len :" << fileName.length() << "  " << endl;
    inFile.open(fileName,ios::in|ios::binary);
    if(!inFile.is_open())
    {
        cout << "open inFile error." << endl;
        send(cfd, "no123file", SIZE, 0); //告诉client 没有这个文件
        return false;
    }
    //计算文件大小
    inFile.seekg(0, ios::end);
    size = inFile.tellg();
    inFile.seekg(0, ios::beg);
    //发送文件大小
    string temp = to_string(size);
    send(cfd, temp.c_str(), SIZE, 0);
    cout << "send fileSize: " << temp << endl;
    int k = 0;
    while(!inFile.eof())
    {
        inFile.read(buf, sizeof(buf));
        //获取当前读指针位置
        int pos = inFile.tellg();
        if(pos == -1)
        {
            pos = size - k;
            ret = send(cfd, buf, pos, 0);
            //cout << "ret : " << ret << endl;
            //读到文件尾部
            break;
        }
        pos -= k;
        k += pos;
        //cout << "buf:" << buf << endl;
        ret = send(cfd, buf, pos, 0);
        //cout << "ret : " << ret << endl;
        memset(buf, 0, sizeof(buf));
    }
    inFile.close();
    cout << "发送文件成功" << endl;
    return true;
}

bool recFile(int &cfd) {
    fstream outFile;
    string fileName;
    int size;
    char buf[SIZE] = { 0 };
    //获取client 上传的文件名
    int num = read(cfd, buf,SIZE);
    if(num == -1)
    {
        cout << "read error" << endl;
        //throw "read";
    }
//    cout << "file name length: " << num << endl;
    fileName = buf;
    cout << "file name: " << fileName << endl;
    memset(buf, 0, sizeof(buf));
    outFile.open(fileName,ios::out|ios::binary);
    if(!outFile.is_open())
    {
        cout << "open file failed....." << endl;
        return false;
    }
    read(cfd, buf, 128);//读取文件大小
    int  fileSize = atoi(buf);
    memset(buf, 0, 128);
    cout << "fileSize: " << fileSize << endl;
    int sizeTmp = 0;
    while((num = read(cfd, buf, sizeof(buf))) > 0 )
    {
        //client 告诉服务器数据发送完成 read是默认阻塞
        //cout << "buf: " << buf << endl;
//        if(strcmp(buf, "break") == 0)
//        {
//            cout << "break" << endl;
//            break;
//        }
        outFile.write(buf, num);
        sizeTmp += num;
        if(fileSize == sizeTmp)
        {
            //cout << "文件写已经写完" << endl;
            break;
        }
        memset(buf, 0, sizeof(buf));
    }
    outFile.close();
    cout <<"文件接收成功" << endl;
    return true;
}

bool sendDirectory(int& cfd, vector<string>& fileName) {
    int size;
    int num;
    char *tmp = "break";
    for(auto name:fileName)
    {
        num = name.length();
//        cout << "length = " << num << endl;
        string str = to_string(num);
//        cout << "str len :" << str.length() << endl;
        //发送文件名称长度
        size = send(cfd, str.c_str(), sizeof(int), 0);
//        cout << "str.c_str() : " << str.c_str() << endl;
//        cout <<"size= "<< size << endl;
//        cout << "name: " << name << endl;
        //发送文件名
        size = send(cfd, name.c_str(), name.length(), 0);
//        cout << "name size:  " << size << endl;
    }
    num = strlen(tmp);
    string str = to_string(num);
    send(cfd, str.c_str(), sizeof(int), 0);
    send(cfd, tmp, num, 0);
    cout << "发送目录成功" << endl;
    return true;
}

//遍历目录
bool scanfDir(char *root, vector<string>& fileName) {
    if(root == nullptr)
    {
        cout << "root is empty...." << endl;
        return false;
    }
    struct stat s;
    lstat(root, &s); //读取路径信息
    //判断是否为目录
    if(!S_ISDIR(s.st_mode))
    {
        return false;
    }
    struct dirent *fileDirent;
    DIR *dir;
    dir = opendir(root);
    if(dir == nullptr)
    {
        cout << "opendir error" << endl;
        return false;
    }
    while((fileDirent = readdir(dir)) != nullptr)
    {
        if(strcmp(fileDirent->d_name, ".") == 0 || strcmp(fileDirent->d_name, "..") == 0)
        {
            continue;
        }
        //char wholePath[1024] = { 0 };
        //sprintf(wholePath,"%s/%s", fileDirent->d_name)
//        cout << fileDirent->d_name << endl;
        fileName.push_back(fileDirent->d_name);
    }
    return true;
}

