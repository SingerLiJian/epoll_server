#ifndef _SOCKET_
#define _SOCKET

#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

class Socket 
{
  protected:
    //服务器的socket号
    int nListenSocket;
    //服务器的端口号
    int m_nServerPort;  
    //ip号
    char* m_strBoundIP;
    //连接fd
    int nConnectedSocket;
    char *recvbuf = new char [100];
    char *sendbuf = new char [100];
    virtual int create() = 0; //获取对应的socket号

  public:
    virtual ~Socket();
    virtual char* readfd(int fd) = 0;//服务器或者客户端读
    virtual void writefd(int fd, const char *buf) = 0;//服务器或者客户端写
};

Socket::~Socket(){
    close(nListenSocket);
    close(nConnectedSocket);
    if(m_strBoundIP != NULL)
    {
        delete [] m_strBoundIP;
    }
}

#endif