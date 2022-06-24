#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "TIME_WHEEL_TIMER.h"
 
#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER   1024
#define TIMESLOT  1 
static int pipefd[2];
static time_wheel tw;
static int epollfd = 0;
 
 
//设置非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
 
void addfd(int epollfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;   //ET模式
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);   //添加到事件列表
    setnonblocking(fd);   //设置非阻塞
    
    return ;
}
 
void sig_handler(int sig)
{
    int save_errno = errno;
    int msg = sig;
    send(pipefd[1], (char*)&msg, 1, 0);
    errno = save_errno;
}
 
//信号处理
void addsig(int sig)
{
    struct sigaction sa;
    (struct sigaction *)memset(&sa, '\0', sizeof(struct sigaction));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
 
    return ;
}
 
//
void timer_handler()
{
    tw.tick();
 
    alarm(TIMESLOT);
}
 
//回调函数
void cb_func(client_data *user_data)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    std::cout << "close fd " << user_data->sockfd << std::endl;
}
 
int main(int argc, char **argv)
{
    if(argc <= 2)   //使用IP port
    {
        std::cout << "usage:" << argv[0] << " ipaddr port" << std::endl;
        return -1;
    }
 
    struct sockaddr_in address;
    bzero(&address, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &address.sin_addr);
    address.sin_port = htons(atoi(argv[2]));
 
 
    //socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd != -1);
 
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    //bind
    int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(struct sockaddr_in));
    assert(ret != -1);
 
    //listen
    ret = listen(listenfd, 5);
    assert(ret != -1);
 
    //事件集合
    struct epoll_event events[MAX_EVENT_NUMBER];
    //列表套接字
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    
    addfd(epollfd, listenfd);  //添加到事件列表
 
    //双向通信
    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
    assert(ret != -1);
    setnonblocking(pipefd[1]);
    addfd(epollfd, pipefd[0]);
 
 
    
    //设置信号处理函数
    addsig(SIGALRM);
    addsig(SIGTERM);
    bool stop_server = false;
 
    client_data *users = new client_data[FD_LIMIT];
    bool timeout = false;
    alarm(TIMESLOT);    //定时
 
    while(!stop_server)
    {
        int num = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if(num < 0 && errno != EINTR)
        {
            std::cout << "epoll failure" << std::endl;
            break;
        }
 
        for(int i = 0; i < num; ++i)
        {
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd)   //新的连接请求
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(struct sockaddr_in);
                int connfd = accept(listenfd, (struct sockaddr*)&client, &len);
                assert(connfd != -1);
 
                addfd(epollfd, connfd);
                users[connfd].address = client;
                users[connfd].sockfd = connfd;
 
                //创建定时器
                tw_timer *timer = tw.add_timer(10);
                timer->user_data = &users[connfd];
                timer->cb_func = cb_func;      //回调函数
                users[connfd].timer = timer;
                break;
            
            }
            else if(sockfd == pipefd[0] && events[i].events & EPOLLIN)  //处理信号
            {
                int sig;
                char signals[1024];
                ret = recv(pipefd[0], signals, sizeof(signals), 0);
                assert(ret != -1);
 
                if(ret == 0)
                    continue;
                else
                {
                    for(int i = 0; i < ret; ++i)
                        switch(signals[i])
                        {
                            case SIGALRM:
                                timeout = true; //有定时任务需要处理
                                break;
                            case SIGTERM:
                                stop_server = true;
                        }
                }
            
            }
            else if(events[i].events & EPOLLIN)  //处理客户连接上接收的数据
            {
                memset(users[sockfd].buf, '\0', BUFFER_SIZE);
                ret = recv(sockfd, users[sockfd].buf, BUFFER_SIZE - 1, 0);
                std::cout << "get " << ret << " bytes os client data " << users[sockfd].buf << " from " << sockfd << std::endl;
 
                tw_timer *timer = users[sockfd].timer;
                if(ret < 0)          //删除定时器  关闭描述符
                {
                    if(errno != EAGAIN)
                    {
                        cb_func(&users[sockfd]);
                        if(timer)
                            tw.del_timer(timer);
                    }
                }
                else if(ret == 0)
                {
                    cb_func(&users[sockfd]);
                    if(timer)
                        tw.del_timer(timer);
                }
                else
                {
                    //调整定时器 延迟关闭
                    if(timer)
                    {
                        time_t cur = time(NULL);
                        timer->rotation = 1;
                        std::cout << "adjust timer once" << std::endl;
                    }
                    
                }
            
            }
            else
            {
                ;
            }
        }
        if(timeout)
        {
            timer_handler();
            timeout = false;
        }
 
    }
 
    close(listenfd);
    close(pipefd[1]);
    close(pipefd[0]);
    delete []users;
 
    return 0;
}
 