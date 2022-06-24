#ifndef _TIME_WHEEL_TIMER
#define _TIME_WHEEL_TIMER
//时间轮
#include <time.h>
#include <netinet/in.h>
#include <iostream>
 
#define BUFFER_SIZE  64
 
class tw_timer;  //前向声明
 
//绑定socket和定时器
struct client_data{
    struct sockaddr_in address;  //addr
    int sockfd;                
    char buf[BUFFER_SIZE];
    tw_timer *timer;
};
 
/********定时器类*********/
class tw_timer{
    public:
        tw_timer(int rot, int ts):rotation(rot),time_slot(ts)
        {}
        
        void (*cb_func)(client_data *);  //定时器回调函数
 
    public:
        int rotation; //时间轮转多少圈后生效
        int time_slot;     //属于时间轮上哪个槽
        client_data *user_data{nullptr};   //客户数据
        tw_timer *next{nullptr};   //指向下一个定时器
        tw_timer *prev{nullptr};   //指向前一个定时器
 
};
 
class time_wheel{
    public:
        time_wheel();
        ~time_wheel();
 
        tw_timer *add_timer(int timeout);   //根据定时值timeout创建一个定时器 插入合适的槽中
        void del_timer(tw_timer *timer);    //删除目标定时器timer
        void tick();                       //SI时间到后，调用该函数 时间轮前滚动一个槽的间隔
 
    private:
        static const int N = 60;  //时间轮上槽的数目
        static const int SI = 1; //槽间隔1S
        tw_timer *slots[N];    //槽
        int cur_slot{0}; //当前槽
};
 
time_wheel::time_wheel()
{
    for(int i = 0; i < N; ++i)
        slots[i] = nullptr;
}
 
time_wheel::~time_wheel()
{
    for(int i = 0; i < N; ++i)
    {
        tw_timer *tmp = slots[i];
        while(tmp)
        {
            slots[i] = tmp->next;
            delete tmp;
            tmp = slots[i];
        }
    }
}
 
//创建定时器
tw_timer* time_wheel::add_timer(int timeout)
{
    if(timeout < 0)
        return nullptr;
    int ticks = 0;
 
    //根据插入定时器的超时值计算它将在时间轮多少个滴答后被触发
    //并将该滴答数存于ticks中
    if(timeout < SI)//最少一秒
        ticks = 1;
    else
        ticks = timeout / SI;//取整数
    //计算多少圈后被触发
    int rotation = ticks / N;
    //计算插入哪个槽中
    int ts = (cur_slot + (ticks % N)) % N;
 
    //创建定时器 在时间轮转动rotation 圈后触发  且位于第ts个槽上 
    tw_timer *timer = new tw_timer(rotation, ts);
 
    if(!slots[ts])
    {
        std::cout << "add timer, rotation is " << rotation << " , ts is " << ts << " cur_slot is " << cur_slot << std::endl;
 
        slots[ts] = timer;
    }
    else
    {
        timer->next = slots[ts];
        slots[ts]->prev = timer;
        slots[ts] = timer;
    }
 
    return timer;
}
 
//删除目标定时器
void time_wheel::del_timer(tw_timer *timer)
{
    if(!timer)
        return;
 
    int ts = timer->time_slot;   //在哪个槽上
 
    if(timer == slots[ts])   //如果槽中第一个结点是要删除结点
    {
        slots[ts] = slots[ts]->next;
        if(slots[ts])
            slots[ts]->prev = nullptr;
        delete timer;
    }
    else
    {
        timer->prev->next = timer->next;
        if(timer->next)
            timer->next->prev = timer->prev;
        timer->next = nullptr;
        timer->prev = nullptr;
        delete timer;
    }
    
    return ;
}
 
//SI时间到后 调用该函数 
void time_wheel::tick()
{
    tw_timer *tmp = slots[cur_slot];  //取得当前槽的头结点
 
    std::cout << "Current slot is " << cur_slot << std::endl;
    while(tmp)
    {
        std::cout << "tick the timer once" << std::endl;
        if(tmp->rotation > 0)
        {
            tmp->rotation--;
            tmp = tmp->next;
        }
        else
        {
            tmp->cb_func(tmp->user_data);
            if(tmp == slots[cur_slot])
            {
                std::cout << "delete handle in cur_slot" << std::endl;
                slots[cur_slot] = tmp->next;
                delete tmp;
                if(slots[cur_slot])
                    slots[cur_slot]->prev = nullptr;
                tmp = slots[cur_slot];
            }
            else
            {
                tmp->prev->next = tmp->next;
                if(tmp->next)
                    tmp->next->prev = tmp->prev;
                tw_timer *tmp2 = tmp->next;
                delete tmp;
                tmp = tmp2;
            }
        }
    }
 
    cur_slot = ++cur_slot % N;
    
    return;
}
 
#endif
 
 