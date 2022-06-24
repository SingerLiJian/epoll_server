#ifndef _epoll_
#define _epoll_

#include <sys/epoll.h>
#include <memory.h>

class Mypoll
{
private:
    int epfd_;
    struct epoll_event *events_;
    struct epoll_event ev_;

public:
    Mypoll(uint32_t m_ = 256);
    ~Mypoll();
    void Ctrl(int op, int fd, uint32_t event);
    int Poll(int maxevents, int timeout);
    struct epoll_event *GetEvents();
};

Mypoll::Mypoll(uint32_t m_ = 256)
{
    events_ = new struct epoll_event[m_];
    memset(events_, 0, sizeof(struct epoll_event));
    epfd_ = epoll_create(m_);

    if(epfd_ == -1){
        fprintf(stderr, "epoll create failed");
    }
}

Mypoll::~Mypoll()
{
    close(epfd_);
    delete [] events_;
}

void Mypoll::Ctrl(int op, int fd, uint32_t event)
{
    ev_.data.fd = fd;
    ev_.events = event;
    epoll_ctl(epfd_, op, fd, &ev_);
}

int Mypoll::Poll(int maxevents, int timeout)
{
    return epoll_wait(epfd_, events_, maxevents, timeout);
}

struct epoll_event* Mypoll::GetEvents()
{
    return events_;
}

#endif