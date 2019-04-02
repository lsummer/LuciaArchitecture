#include "cia_socket.h"
// 存在惊群效应
void CSocket::cia_socket_accept(Kevent_Node* kn){
    //  accept 也是要设置为非阻塞的，且建立完一个连接就要把这个连接加入到epoll中去
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    
    int accept_fd = accept(kn->fd, (struct  sockaddr *)&client_addr, &len);

    if(accept_fd == -1){  // 这里有多种情况
        int err = errno;
        if(err == EAGAIN || err == EWOULDBLOCK){ // 再试一次，不算错误
            return; 
        }
        if (err == ECONNABORTED)  //ECONNRESET错误则发生在对方意外关闭套接字后【您的主机中的软件放弃了一个已建立的连接--由于超时或者其它失败而中止接连(用户插拔网线就可能有这个错误出现)】
        {
            //该错误被描述为“software caused connection abort”，即“软件引起的连接中止”。原因在于当服务和客户进程在完成用于 TCP 连接的“三次握手”后，
                //客户 TCP 却发送了一个 RST （复位）分节，在服务进程看来，就在该连接已由 TCP 排队，等着服务进程调用 accept 的时候 RST 却到达了。
                //POSIX 规定此时的 errno 值必须 ECONNABORTED。源自 Berkeley 的实现完全在内核中处理中止的连接，服务进程将永远不知道该中止的发生。
                    //服务器进程一般可以忽略该错误，直接再次调用accept。
            LOG_ERR(ERROR, "cia_socket_accept() 中的 accpet()出现错误errno = ECONNABORTED, 直接返回");
        } 
        else if (err == EMFILE || err == ENFILE) //EMFILE:进程的fd已用尽【已达到系统所允许单一进程所能打开的文件/套接字总数】。可参考：https://blog.csdn.net/sdn_prc/article/details/28661661   以及 https://bbs.csdn.net/topics/390592927
                                                    //ulimit -n ,看看文件描述符限制,如果是1024的话，需要改大;  打开的文件句柄数过多 ,把系统的fd软限制和硬限制都抬高.
                                                //ENFILE这个errno的存在，表明一定存在system-wide的resource limits，而不仅仅有process-specific的resource limits。按照常识，process-specific的resource limits，一定受限于system-wide的resource limits。
        {
            
            LOG_ERR(ERROR, "cia_socket_accept() 中的 accpet()出现错误errno = EMFILE | ENFILE， 直接返回");
        }
        return; 
    }
    if(!setnoneblocking(accept_fd)){
        close(accept_fd);
        LOG_ERR(ERROR, "CSocket::cia_socket_accept() 中 setnoneblocking() 设置非阻塞失败，直接退出！");
        return;
    }
    
    int current_link =  fd_ports.size();
    FD_PORT* fd_port = new FD_PORT(accept_fd,true, 0);

    // TODO 处理连接请求发来的数据
    if(cia_add_epoll(fd_port, fd_port->wr_flag?1:0, fd_port->wr_flag?0:1,  &CSocket::cia_wait_request_handler) == false){
        delete fd_port;
        return;
    } 
    fd_ports.push_back(fd_port);

    // accept_fd 说明连接已经建立，要将该连接加入到kevent里面去
}