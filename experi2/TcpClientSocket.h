#ifndef _TcpClientSocket_
#define _TcpClientSocket_

#include "TcpSocket.h"

class TcpClientSocket:public TcpSocket
{
private:

public:
    TcpClientSocket(int nServerPort, const char *strServerIP);
    virtual ~TcpClientSocket();
    int connect();   // 连接
    int getClientFd();
};


TcpClientSocket::TcpClientSocket(int nServerPort, const char *strServerIP)
{
    m_nServerPort = nServerPort;
    int nlength = strlen(strServerIP);
    m_strBoundIP = new char [nlength + 1];
    strcpy(m_strBoundIP, strServerIP);
    //调用tcp父类的函数产生socket
    nConnectedSocket = create();
    if(-1 == nConnectedSocket)
    {
        std::cout << "socket error" << std::endl;
    }
}


int TcpClientSocket::connect(){
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
    if(::connect(nConnectedSocket, (sockaddr*)&ServerAddress, sizeof(ServerAddress)) == -1)
    {
        std::cout << "connect error" << std::endl;
        ::close(nConnectedSocket);
        return -1;
    }
    return 0;
}

int TcpClientSocket::getClientFd(){
    return nConnectedSocket;
}


TcpClientSocket::~TcpClientSocket(){
}

#endif