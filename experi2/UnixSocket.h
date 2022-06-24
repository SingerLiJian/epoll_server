#ifndef _UnixSocket_
#define _UnixSocket_

#include "Socket.h"

class UnixSocket:public Socket
{
protected:
    int create();
    char* m_strPath;
public:
    virtual ~UnixSocket();
    char* readfd(int fd);
    void writefd(int fd, const char *buf);
};


int UnixSocket::create(){
    int socket = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if(-1 == socket)
    {
        std::cout << "socket error" << std::endl;
        return -1;
    }
    return socket;
}

char* UnixSocket::readfd(int fd){
    memset(recvbuf,0,sizeof(recvbuf));
    int r = read(fd, recvbuf, sizeof(recvbuf));
    if(r <= 0){
        char *nullBuf = "wrong";//代表无内容
        return nullBuf;
    }
    return recvbuf;
}

void UnixSocket::writefd(int fd, const char *buf){
    write(fd, buf, strlen(buf));
}

#endif