//
// Created by 汤汤 on 2021/2/3.
//

#include "work.h"
#include <vector>
#include <sys/stat.h>
#include <dirent.h>

bool working(Socket& socketfd, int flag) {
    vector<string> name;
    int depth = DEPTH;
    int m_time[64] = { 0 };

    switch(flag)
    {
        //获取服务器文件目录
        case GETDIR:
            //scanfDir("./", name);
            getCurrentTime();
            recursiveScanfDir(".", depth, name);
            mySort2(name);
            sendDirectory(socketfd.getCfd(),name);
            break;
        //传送文件给Clint
        case SENDFILE:
            getCurrentTime();
            sendFile(socketfd.getCfd());
            break;
        //接受client文件
        case RECFILE:
            getCurrentTime();
            recFile(socketfd.getCfd());
            break;
        default :
            getCurrentTime();
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
    size = read(cfd, buf, 1024);
    cout << "read size :  " << size << endl;
    fileName = buf;
    cout << "fileName: "<< fileName << endl;
    //cout << "len :" << fileName.length() << "  " << endl;
    inFile.open(fileName,ios::in|ios::binary);
    if(!inFile.is_open())
    {
        cout << "open inFile error." << endl;
        send(cfd, "no123file", 1024, 0); //告诉client 没有这个文件
        return false;
    }
    //计算文件大小
    inFile.seekg(0, ios::end);
    size = inFile.tellg();
    inFile.seekg(0, ios::beg);
    //发送文件大小
    string temp = to_string(size);
    send(cfd, temp.c_str(), 1024, 0);
    cout << "send fileSize: " << temp << endl;
    int k = 0;
    while(!inFile.eof())
    {
        inFile.read(buf, sizeof(buf));
        //获取当前读指针位置
        int pos = inFile.tellg();
//        cout << "pos1 : " << pos << endl;
        //读到文件尾部
        if(pos == -1)
        {
            pos = size - k; //最后一次读取读值大小
            ret = send(cfd, buf, pos, 0);
//            cout << buf << endl;
            if(ret == -1)
            {
                break;
            }
            cout << "ret-1  : " << ret << endl;
            break;
        }
        pos -= k; // 获取read读的大小
        k += pos;
//        cout << "pos2: " << pos << endl;
        //cout << "buf:" << buf << endl;
//        cout << buf << endl;
        ret = send(cfd, buf, pos, 0);
        if(ret == -1)
        {
            break;
        }
//        cout << "ret : " << ret << endl;
        memset(buf, 0, sizeof(buf));
    }

    inFile.close();
    cout << fileName <<": 发送文件成功" << endl;
    return true;
}

bool recFile(int &cfd) {
    fstream outFile;
    string fileName;
    int size;
    char buf[SIZE] = { 0 };
    //获取client 上传的文件名
    int num = read(cfd, buf,1024);
    if(num == -1)
    {
        cout << "read error" << endl;
        //throw "read";
    }
//    cout << "file name length: " << num << endl;
    fileName = buf;
    cout << "file name: " << fileName << endl;
    memset(buf, 0, sizeof(buf));
    //保存文件到上级save目录
    outFile.open("../save/"+fileName,ios::out|ios::binary);
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
            cout << fileName <<":文件接收成功" << endl;
//            cout << "文件写已经写完" << endl;
            break;
        }
        memset(buf, 0, sizeof(buf));
    }
    char *bufTmp = "sendfileok";
//    cout << bufTmp <<"size of" << sizeof(bufTmp) << endl;
    size = send(cfd, bufTmp, 11, 0);
    if(size == -1)
    {
        cout << "-1**************" << endl;
    }
    //cout << "size : "<< size << endl;
    outFile.close();
    return true;
}
//发送文件夹
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
        if(size == 0)
        {
            cout << "send dir lenth error" << endl;
        }
//        cout << "str.c_str() : " << str.c_str() << endl;
//        cout <<"size= "<< size << endl;
//        cout << "name: " << name << endl;
        //发送文件名
        size = send(cfd, name.c_str(), name.length(), 0);
        if(size == 0)
        {
            cout << "send dir name error" << endl;
        }
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
    DIR *dir = opendir(root);
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
        //cout << fileDirent->d_name << endl;
        fileName.push_back(fileDirent->d_name);
    }
    return true;
}

bool recursiveScanfDir(const char *root, int depth, vector<string> &fileName) {
    struct stat s;
    stat(root, &s);
    if(!S_ISDIR(s.st_mode))
    {
        cout << "root is not directory" << endl;
        return false;
    }
    struct dirent *entry;
    DIR *dir = opendir(root);
    int num = 1;
    while((entry= readdir(dir)) != nullptr)
    {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        char src[255] = { 0 };
        strcpy(src, root);
        strcat(src, "/");
        strcat(src, entry->d_name);
        struct stat statdir;
        stat(src, &statdir);
        if(statdir.st_mode & S_IFDIR)
        {
            string aa = "\"文件夹:\" ";
            aa += src;
            fileName.push_back(aa);
            goto Later;
        }
        fileName.push_back(src);
      Later:
        if(statdir.st_mode & S_IFDIR)
        {
            num--;
            if(num == 0)
            {
                depth--;
            }
            if(depth >= 1)
            {
                recursiveScanfDir(src, depth, fileName);
            }
        }
    }
    closedir(dir);
    return true;
}

int mySort(string str)
{
    int number = 0;
    int pos = str.find("/", 0);
    while(pos != string::npos)
    {
        pos++;
        pos = str.find("/", pos);
        number++;
    }
    return number;
}

bool mySort2(vector<string> &fileName)
{
    vector<string> tmp;
    vector<string> tmp1;
    vector<string> tmp2;
    vector<string> tmp3;
    vector<string> tmp4;
    tmp.swap(fileName); //交换
    for(auto name:tmp)
    {
        int num = mySort(name);
        if(num == 2)
        {
            tmp2.push_back(name);
        }
        if(num == 1)
        {
            tmp1.push_back(name);
        }
        if(num == 3)
        {
            tmp3.push_back(name);
        }
    }
    sort(tmp1.begin(), tmp1.end());
    sort(tmp3.begin(), tmp3.end());
    sort(tmp2.begin(), tmp2.end());
    tmp1.insert(tmp1.begin(),"=======================第一层=======================");
    tmp2.insert(tmp2.begin(),"=======================第二层=======================");
    //tmp3.insert(tmp3.begin(),"=======================第三层=======================");
    tmp4.push_back("=======================第三层=======================");
    for(auto a:tmp1) {
        fileName.push_back(a);
    }
    for(auto a:tmp2){
        fileName.push_back(a);
    }
    for(auto a2: tmp2)
    {
        int pos = a2.find(" ");
        //cout << "pos:" << pos << endl;
        if(pos != -1)
        {
            pos = a2.rfind("/");
            string mstr = "*******************************文件夹:" + a2.substr(pos+1, a2.length()) + "内容*******************************";
            tmp4.push_back(mstr);
            //cout << "------------" << a2 << "-----------" << endl;
            for(auto a3:tmp3)
            {
                int mpos = a3.find(a2.substr(pos+1, a2.length()));
                if(mpos != -1){
                    tmp4.push_back(a3);
                    //cout << a3 << endl;
                }else{
                    //cout << "这个" << a2 << "为空" << endl;
                }
            }
        }
    }
    for(auto a:tmp4){
        fileName.push_back(a);
    }
    return true;
}

void getCurrentTime()
{
    time_t t = time(nullptr);
    char buf[64] = { 0 };
    strftime(buf, sizeof(buf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));
    cout << buf << endl;
}