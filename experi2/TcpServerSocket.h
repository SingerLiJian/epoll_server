#ifndef _TcpServerSocket_
#define _TcpServerSocket_

#include "TcpSocket.h"

class TcpServerSocket:public TcpSocket
{
private:
	//接听的数量上限
    int m_nLengthOfQueueOfListen;
public:
    TcpServerSocket(int nServerPort, int nLengthOfQueueOfListen , const char *strBoundIP);
    virtual ~TcpServerSocket();
    int listenbind();
    int accept(int flag);//flag==1为阻塞，0为非阻塞
};

TcpServerSocket::TcpServerSocket(int nServerPort, int nLengthOfQueueOfListen = 100, const char *strBoundIP = NULL){
    m_nServerPort = nServerPort;
    m_nLengthOfQueueOfListen = nLengthOfQueueOfListen;
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
}

int TcpServerSocket::listenbind(){
    nListenSocket = create();
    if(-1 == nListenSocket)
    {
        std::cout << "socket error" << std::endl;
        return -1;
    }

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
    //listen状态
    if(::listen(nListenSocket, m_nLengthOfQueueOfListen) == -1)
    {
        std::cout << "listen error" << std::endl;
        ::close(nListenSocket);
        return -1;
    }
    return nListenSocket;
}

int TcpServerSocket::accept(int flag){
    //准备accept
    sockaddr_in ClientAddress;
    socklen_t LengthOfClientAddress = sizeof(sockaddr_in);
    if (flag == 1)
    {
        nConnectedSocket = ::accept(nListenSocket, (sockaddr *)&ClientAddress, &LengthOfClientAddress);
    }
    else{
        nConnectedSocket = ::accept4(nListenSocket, (sockaddr *)&ClientAddress, &LengthOfClientAddress, SOCK_NONBLOCK);
    }
    if(-1 == nConnectedSocket)
    {
        std::cout << "accept error" << std::endl;
        ::close(nListenSocket);
        return -1;
    }
    return nConnectedSocket;
}

TcpServerSocket::~TcpServerSocket(){
}

#endif