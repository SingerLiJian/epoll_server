#ifndef _UdpServerSocket_
#define _UdpServerSocket_

#include "UdpSocket.h"

class UdpServerSocket:public UdpSocket
{
private:

public:
    UdpServerSocket(int nServerPort, const char *strBoundIP = NULL);
    virtual ~UdpServerSocket();
    int listenbind();
};


UdpServerSocket::UdpServerSocket(int nServerPort,const char *strBoundIP = NULL){
    m_nServerPort = nServerPort;
    if(NULL == strBoundIP)
    {
        m_strBoundIP = NULL;
    }
    else
    {
        int length = strlen(strBoundIP);
        m_strBoundIP = new char[length + 1];
        memcpy(m_strBoundIP, strBoundIP, length + 1);
    }
    nListenSocket = create();
    if(-1 == nListenSocket)
    {
        std::cout << "socket error" << std::endl;
        return -1;
    }
}

int UdpServerSocket::listenbind(){
    sockaddr_in ServerAddress;
    memset(&ServerAddress, 0, sizeof(sockaddr_in));
    ServerAddress.sin_family = AF_INET;

    if(NULL == m_strBoundIP)
    {
        ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        if(::inet_pton(AF_INET, m_strBoundIP, &ServerAddress.sin_addr) != 1)
        {
        std::cout << "inet_pton error" << std::endl;
        ::close(nListenSocket);
        return -1;
        }
    }

    ServerAddress.sin_port = htons(m_nServerPort);
    //绑定socket
    if(::bind(nListenSocket, (sockaddr *)&ServerAddress, sizeof(sockaddr_in)) == -1)
    {
        std::cout << "bind error" << std::endl;
        ::close(nListenSocket);
        return -1;
    }
}

#endif