#include "cia_socket.h"

CSocket::CSocket(){}
CSocket::~CSocket(){
    closesocket();
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
            LOG_ERR(ERROR, "CSocket::socket_init() 中 socket() 创建失败，直接退出！", (i+1));
            return false;
        }
        int optval = 1; // 表示使用设定，为0表示不使用设定
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(optval)) == -1){
            close(sock);
            LOG_ERR(ERROR, "CSocket::socket_init() 中 setsockopt() 设置失败，直接退出！", (i+1));
            return false;
        }
        if(!setnoneblocking(sock)){
            close(sock);
            LOG_ERR(ERROR, "CSocket::socket_init() 中 setnoneblocking() 设置非阻塞失败，直接退出！", (i+1));
            return false;
        }

        serv_addr.sin_port = htons(port1); // htons = uint16_t, htonl = uint32_t
        if(bind(sock, (struct  sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
            close(sock);
            LOG_ERR(ERROR, "CSocket::socket_init() 中 bind() 绑定端口失败，直接退出！", (i+1));
            return false;
        }
        int backlog = 1024;
        if(listen(sock, backlog) == -1){
            close(sock);
            LOG_ERR(ERROR, "CSocket::socket_init() 中 listen() 监听端口失败，直接退出！", (i+1));
            return false;
        }
        FD_PORT* fd_port = new FD_PORT(sock, port1);
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
        delete fd_ports[i];
        fd_ports[i] = NULL;
    }
}