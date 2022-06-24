#include "TcpServerSocket.h"

#define SERVERPORT 8888
#define SERVERIP "127.0.0.1"

int main(){
    TcpServerSocket* server = new TcpServerSocket(SERVERPORT, 100, SERVERIP);
    server->listenbind();
    cout<<"这是一个服务器"<<endl;
    int clientFd = server->accept(1);
    while(1){
        char *recvm = server->readfd(clientFd);
        cout<<"接收到消息："<<recvm<<endl;
        server->writefd(clientFd, recvm);
    }
}