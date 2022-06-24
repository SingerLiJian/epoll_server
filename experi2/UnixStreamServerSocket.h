#ifndef _UnixStreamServerSocket_
#define _UnixStreamServerSocket_

#include "UnixSocket.h"

class UnixStreamServerSocket:public UnixSocket
{
private:
    int m_nLengthOfQueueOfListen;
public:
    UnixStreamServerSocket(const char *strPath);
    virtual ~UnixStreamServerSocket();
    int listenbind();
    int accept();
};


UnixStreamServerSocket::UnixStreamServerSocket(int length, const char *strPath){
    m_nLengthOfQueueOfListen = length;
    m_strPath = strPath;
    int nlength = strlen(strPath);
    m_strPath = new char [nlength + 1];
    strcpy(m_strPath, strPath);
}

int UnixStreamServerSocket::listenbind(){
    nListenSocket = create();
    if(-1 == nListenSocket)
    {
        std::cout << "socket error" << std::endl;
        return -1;
    }

    sockaddr_un s_un;
    s_un.sun_family = AF_UNIX;
    strcpy(s_un.sun_path, m_strPath);
 
    if(bind(nListenSocket, (sockaddr*)&s_un, sizeof(s_un)) != 0){
        perror("bind");
        return 1;
    }

    if(listen(nListenSocket, m_nLengthOfQueueOfListen) != 0){
        perror("listen");
        return 1;
    }
}

int UnixStreamServerSocket::accept(){
    sockaddr_un s_un_accept;
    socklen_t addrlen = sizeof(s_un_accept);
    nConnectedSocket = accept(nListenSocket, (sockaddr*)&s_un_accept, &addrlen);
    if(nConnectedSocket < 0){
        perror("accept");
        return 1;
    }
    return nConnectedSocket;
}

#endif