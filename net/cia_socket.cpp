#include "cia_socket.h"
#include "cia_kernal_func.h"
#include <sys/event.h>

CSocket::CSocket(){}
CSocket::~CSocket(){
    closesocket();
    delete free_link;
}

// 返回值表示是否创建成功一个，如果一个都没创建成功，直接退出程序
bool CSocket::socket_init(){
    // socket()
    // setsocketopt()设置为非阻塞，且为了避免Time_wait导致的端口已被占用的情况，使用SO_REUSEADDR
    // 循环bind()指定的端口
    // 然后listen()端口，但是注意的是listen的监听的文件描述符是要放到epoll中去的，每次epoll_wait获得连接时，通过对应的动作（accept()）来处理
    int sock;
    bool init_success = false;

    CConfig *conf = CConfig::getInstance();
    int ports = conf->GetIntDefault("PortNumber"); // port的数量

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;  // 表示ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 表示服务器上任意网卡的ip地址
    
    for(int i=0; i<ports; i++){
        int port1 = conf->GetIntDefault("Port"+std::to_string(i+1)); // 端口

        if(port1 < 0){   // 表示没有这个，所以要加个Warning
            LOG_ERR(WARN, "配置文件中的端口 Port%d 出现错误, 已经跳过该端口的监听。", (i+1));
            continue;
        }
        sock = socket(AF_INET, SOCK_STREAM, 0); // TCP ipv4固定组合
        if(sock == -1){  // 程序直接退出
            LOG_ERR(ERROR, "CSocket::socket_init() 中 socket() 创建失败，直接退出！");
            return false;
        }
        int optval = 1; // 表示使用设定，为0表示不使用设定
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(optval)) == -1){
            close(sock);
            LOG_ERR(ERROR, "CSocket::socket_init() 中 setsockopt() 设置失败，直接退出！");
            return false;
        }
        if(!setnoneblocking(sock)){
            close(sock);
            LOG_ERR(ERROR, "CSocket::socket_init() 中 setnoneblocking() 设置非阻塞失败，直接退出！");
            return false;
        }

        serv_addr.sin_port = htons(port1); // htons = uint16_t, htonl = uint32_t
        if(bind(sock, (struct  sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
            close(sock);
            LOG_ERR(ERROR, "CSocket::socket_init() 中 bind() 绑定端口失败，直接退出！");
            return false;
        }
        int backlog = 1024;
        if(listen(sock, backlog) == -1){
            close(sock);
            LOG_ERR(ERROR, "CSocket::socket_init() 中 listen() 监听端口失败，直接退出！");
            return false;
        }
        FD_PORT* fd_port = new FD_PORT(sock,true, port1);
        fd_ports.push_back(fd_port);

        init_success = true;
        LOG_ERR(INFO, "端口 %d 监听成功！", port1);

    }
    if(init_success){
        return true;
    }
    return false;
    // socket()

}

bool CSocket::setnoneblocking(int fd){
    int flags;
    if(( flags =  fcntl(fd, F_GETFL, 0)) < 0){
        return false;
    }
    flags |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) < 0){
        return false;
    }
    return true;
}

void CSocket::closesocket(){
    for(int i=0; i<fd_ports.size(); i++){
        close(fd_ports[i]->fd);
        if(fd_ports[i]->event != NULL){
            // fd_ports[i]->event->clear();  // 所有和event先关的删除操作要先通过一部clear。
            // delete fd_ports[i]->event;
            free_link->insert_Node(fd_ports[i]->event);
            fd_ports[i]->event = NULL;
        }
        
        delete fd_ports[i];
        fd_ports[i] = NULL;
    }
}

bool CSocket::epoll_init(){ //epoll的使用是linux only 而 macos是unix-like的，所以使用的是kqueue;
    #ifdef  __linux__  // linux操作系统 epoll
        return epoll_init_linux();
    #elif __APPLE__    // macos操作系统 kqueue
        return epoll_init_macos();
    #else
        return fasle;
    #endif
}

bool CSocket::epoll_init_macos(){
    kqueue_fd = kqueue();
    // LOG_ERR(INFO, "worker进程pid=%d中CSocket::epoll_init()设置成功！",getpid()) ;
        
    if(kqueue_fd == -1){
        LOG_ERR(ERROR, "worker进程pid=%d中CSocket::epoll_init() 中 kqueue() 监听端口失败，直接退出！",getpid()) ;
        return false;
    }
    work_connection = CConfig::getInstance()->GetIntDefault("Worker_connection");
    
    free_link = new FreeLink(work_connection);
    struct kevent* kevents = new struct kevent[fd_ports.size()];
    
    // struct kevent event_list[node_number];  // kevent返回的数据结构
    int er_judge = -1;
    for(int i=0; i<fd_ports.size(); i++){
        FD_PORT*  fd_port = fd_ports[i];
        if(cia_add_epoll(fd_port, fd_port->wr_flag?1:0, fd_port->wr_flag?0:1, &CSocket::cia_socket_accept) == false){
            er_judge = i;
            break;
        }
    }
    if(er_judge >= 0){
        // 在这里面把内存释放了
        for(int i=0; i<er_judge; i++){
            FD_PORT*  fd_port = fd_ports[i];
            if(fd_port->event != NULL){
                // fd_port->event->clear();  // 所有和event先关的删除操作要先通过一部clear。
                free_link->insert_Node(fd_port->event);
                // delete fd_port->event;
                fd_port->event = NULL;
            }
        }
        close(kqueue_fd);
        return false;
    }
    // 对所有监听端口添加到对应的ev中去
    // int sock = 1;
    // struct kevent ev;

    // EV_SET(&ev, sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
    // LOG_ERR(INFO, "worker进程pid=%d中CSocket::epoll_init()设置成功，函数退出！",getpid()) ;
    return true;
    // epoll_create(work_connection); // work_connection是指epoll红黑树的节点个数，大于0即可，实际上这个数字已经没有用了
}
bool CSocket::epoll_init_linux(){
    return false;
}

// 添加一个文件描述符到kqueue中去，向fd_ports中加入一个,该函数并没有加，所以要在外面加，然后申请一个free_node
// 参考accept，一定要在fd_ports中加入一条
// read 表示读描述符，write表示写描述符，handler为回调函数
bool CSocket::cia_add_epoll(FD_PORT* fd_port, int read, int write, cia_event_handler_ptr handler, Response* response){
    Kevent_Node* kevent_node = free_link->get_Node();
    if(kevent_node == NULL){
        return false;
    }

    // fd_ports.push_back(fd_port);

    kevent_node->fd = fd_port->fd;
    kevent_node->port = fd_port->port;
    kevent_node->handler = handler; // 应该为处理连接的函数
    kevent_node->c_response = response;
    
    fd_port->event = kevent_node;

    if(read == 1){
        struct kevent kev1;
        EV_SET(&kev1, fd_port->fd, EVFILT_READ, EV_ADD | EV_ENABLE, NULL, 0, kevent_node);
        if(kevent(kqueue_fd, &kev1, 1, NULL, 0, NULL)<0){
            return false;
        }
    }
    if(write == 1){
        struct kevent kev2;
        EV_SET(&kev2, fd_port->fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, NULL, 0, kevent_node);
        if(kevent(kqueue_fd, &kev2, 1, NULL, 0, NULL)<0){
            return false;
        }
    }
    return true;
}
// 删除一个文件描述符时,清楚掉该fd_ports的元素，将该fd_port放回到free_link
void CSocket::cia_del_epoll(int fd, bool wr_flag){
    if(fd < 1) return;

    struct kevent kev2;
    
    auto itre = fd_ports.begin();
    for(; itre != fd_ports.end(); itre++){
        if((*itre)->fd == fd && (*itre)->wr_flag == wr_flag){
            break;
        }
    }
    // LOG_ERR(WARN, "cia_del_epoll()是否出现错误：%d", itre == fd_ports.end())
    
    if(itre != fd_ports.end()){
        
        EV_SET(&kev2, fd, (*itre)->wr_flag?EVFILT_READ:EVFILT_WRITE, EV_DELETE, NULL, 0, (*itre)->event);
        kevent(kqueue_fd, &kev2, 1, NULL, 0, NULL);
        free_link->insert_Node((*itre)->event);  // 回收连接池

        (*itre)->event = NULL;
        delete (*itre);
        (*itre) = NULL;
        fd_ports.erase(itre);
    }
}
