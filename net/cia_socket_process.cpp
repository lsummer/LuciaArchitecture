#include "cia_socket.h"
void CSocket::cia_epoll_process_events(const struct timespec *timeout){
    struct kevent events[work_connection];
    int nevents = 0;
    
    nevents = kevent(kqueue_fd, NULL, 0, events, work_connection, timeout); 
    
    if(nevents < 0){
        // 一堆判断条件先不管
        LOG_ERR(ERROR, "CSocket::cia_epoll_process_events()中kevent()返回值为负数，出错了！");
        return;
    }
   
    for(int i = 0; i<nevents; i++){
        Kevent_Node* eve = (Kevent_Node *)events[i].udata;
        
        (this->*(eve->handler))(eve);
    }
}