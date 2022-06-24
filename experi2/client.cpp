#include "TcpClientSocket.h"

#define SERVERPORT 8888
#define SERVERIP "127.0.0.1"

int main(){
    TcpClientSocket *client = new TcpClientSocket(SERVERPORT, SERVERIP);
    cout<<"这是一个客户端"<<endl;
    client->connect();
    cout<<"连接服务器..."<<endl;
    while (1)
    {
        char* recvm;
        cout<<"请输入内容：";
        cin>>recvm;
        int a = strcmp(recvm,"exit");
        if (a == 0)
        {
            close(client->getClientFd());
            break;
        }
        client->writefd(client->getClientFd(), recvm);
        recvm = client->readfd(client->getClientFd());
        cout<<"服务器返回："<<recvm<<endl;
    }
}