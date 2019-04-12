#include "cia_socket.h"

void CSocket::cia_epoll_process_events(const struct timespec *timeout){
    #ifdef  __linux__  // linux操作系统 epoll
        cia_epoll_process_events_linux(timeout);
    #elif __APPLE__    // macos操作系统 kqueue
        cia_epoll_process_events_macos(timeout);
    #else
        return ;
    #endif
}

#ifdef __linux__
void CSocket::cia_epoll_process_events_linux(const struct timespec *timeout){
    
    struct epoll_event events[work_connection];
    int nevents = 0;
    nevents = epoll_wait(kqueue_fd, events, work_connection, -1);
    if(nevents < 0){
        // 一堆判断条件先不管
        LOG_ERR(ERROR, "CSocket::cia_epoll_process_events()的epoll_wait()返回值为负数，出错了！");
        return;
    }
    for(int i=0; i<nevents; i++){
        Kevent_Node* eve = (Kevent_Node *)(events[i].data.ptr);
        (this->*(eve->handler))(eve);
    }
}
#endif
#ifdef __APPLE__ 
void CSocket::cia_epoll_process_events_macos(const struct timespec *timeout){
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
#endif