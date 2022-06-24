#ifndef _TcpSocket_
#define _TcpSocket_

#include "Socket.h"

class TcpSocket:public Socket
{
protected:
    int create();
public:
    virtual ~TcpSocket();
    char* readfd(int fd);
    void writefd(int fd, const char *buf);
};

int TcpSocket::create(){
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == socket)
    {
        std::cout << "socket error" << std::endl;
        return -1;
    }
    return socket;
}

TcpSocket::~TcpSocket(){
}

char* TcpSocket::readfd(int fd){
    memset(recvbuf,0,sizeof(recvbuf));
    int result_len=read(fd, recvbuf, sizeof(recvbuf));
    if(result_len <= 0){
        char *nullBuf = "wrong";//代表无内容
        return nullBuf;
    }
    return recvbuf;
}

void TcpSocket::writefd(int fd, const char *buf){
    write(fd,buf,strlen(buf));
}

#endif