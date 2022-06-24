#ifndef _UdpClientSocket_
#define _UdpClientSocket_

#include "UdpSocket.h"

class UdpClientSocket:public UdpSocket
{
private:
    
public:
    UdpClientSocket(int nServerPort, const char *strServerIP);
    virtual ~UdpClientSocket();
    int connect();   // 连接
};


UdpClientSocket::UdpClientSocket(int nServerPort, const char *strServerIP)
{
    m_nServerPort = nServerPort;
    int nlength = strlen(strServerIP);
    m_strBoundIP = new char [nlength + 1];
    strcpy(m_strBoundIP, strServerIP);
    //调用Udp父类的函数产生socket
    nConnectedSocket = create();
    if(-1 == nConnectedSocket)
    {
        std::cout << "socket error" << std::endl;
    }
}


int UdpClientSocket::connect(){
    sockaddr_in ServerAddress;
    memset(&ServerAddress, 0, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    if(::inet_pton(AF_INET, m_strBoundIP, &ServerAddress.sin_addr) != 1)
    {
        std::cout << "inet_pton error" << std::endl;
        ::close(nConnectedSocket);
        return -1;
    }
    ServerAddress.sin_port = htons(m_nServerPort);
}

#endif