#ifndef _UdpSocket_
#define _UdpSocket_

#include "Socket.h"

class UdpSocket:public Socket
{
protected:
    int create();
public:
    virtual ~UdpSocket();
    char* readfd(int fd);
    void writefd(int fd, const char *buf);
};


int UdpSocket::create(){
    int socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == socket)
    {
        std::cout << "socket error" << std::endl;
        return -1;
    }
    return socket;
}

char* UdpSocket::readfd(int fd){
	struct sockaddr_in cAddr = {0};
	int len = sizeof(cAddr);
    memset(recvbuf,0,sizeof(recvbuf));
    int r = recvfrom(fd,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&cAddr,&len);
    if(r <= 0){
        char *nullBuf = "wrong";//代表无内容
        return nullBuf;
    }
    return recvbuf;
}

void UdpSocket::writefd(int fd, const char *buf){
    struct sockaddr_in cAddr = {0};
	int len = sizeof(cAddr);
    sendto(fd,buf,strlen(buf),0,(struct sockaddr*)&cAddr,sizeof(cAddr));
}

#endif