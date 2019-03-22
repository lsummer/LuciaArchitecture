#include "cia_kernal_func.h"
#include "cia_global.h"
void cia_process_events_and_timers(){
    
    socket_ctl.cia_epoll_process_events();
}