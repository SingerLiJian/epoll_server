#ifndef _UnixStreamClientSocket_
#define _UnixStreamClientSocket_

#include "UnixSocket.h"

class UnixStreamClientSocket:public UnixSocket
{
private:
    
public:
    UnixStreamClientSocket(const char *strPath);
    virtual ~UnixStreamClientSocket();
    int connect();   // 连接
};


UnixStreamClientSocket::UnixStreamClientSocket(const char *strPath)
{
    m_strPath = strPath;
    int nlength = strlen(strPath);
    m_strPath = new char [nlength + 1];
    strcpy(m_strPath, strPath);
    nConnectedSocket = create();
    if(-1 == nConnectedSocket)
    {
        std::cout << "socket error" << std::endl;
    }
}


int UnixStreamClientSocket::connect(){
    sockaddr_un s_un;
    s_un.sun_family = AF_UNIX;
    strcpy(s_un.sun_path, m_strPath);
    if(connect(nConnectedSocket, (sockaddr*)&s_un, sizeof(s_un)) != 0){
        perror("connect");
        return 1;
    }
}

#endif