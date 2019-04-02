#include "cia_socket.h"
#include "cia_global.h"

#define DATA_LEN 20

void CSocket::cia_wait_request_handler(Kevent_Node* kn){
    
    
    // LOG_ERR(INFO, "pid = %d 来了数据了来了数据了", getpid());
    char buffer[101];
    memset(buffer, 0, 101);
    int n = recv(kn->fd, buffer, 100, 0);
    if(n == 0){  // 表示客户端断开了连接
        //断开连接
        LOG_ERR(WARN, "断开连接");
        cia_del_epoll(kn->fd);
        return;
    }
    if(n < 0){
        int err = errno;
        if(err == EAGAIN || err == EWOULDBLOCK){// 不算错误
            LOG_ERR(WARN,"收到EAGAIN");
            return;
        }
        if(err == EINTR){
            LOG_ERR(WARN, "收包被信号打断");
            return;
        }
        if(err == ECONNRESET){ // 客户端强行关闭
            LOG_ERR(WARN, "客户端强行关闭");
        }else{
            LOG_ERR(WARN, "打印看看出现什么错误：err=%d", err);
        }
        cia_del_epoll(kn->fd);
        return;
    }

    if(kn->status == PKG_UNUSE){
        LOG_ACC(ERROR, "连接池已经被回收，又被使用");
        return;
    }else if(kn->status == PKG_INIT){  // 只有第一次使用的时候是PKG_INIT，所有后续的操作都需要是PK_INIT的后面的步骤
        char *buf =  new char[DATA_LEN+1];
        memset(buf, 0, DATA_LEN+1);
        kn->buf = buf;
        if(n < DATA_LEN){
            memcpy(buf, buffer, n);
            kn->status = PKG_BD_RECVING;
        }else if(n == DATA_LEN){
            memcpy(buf, buffer, n);
            kn->status = PKG_BD_FINISH;
        }else{
            LOG_ACC(ERROR, "不合法数据！");
            return;
        }
    }else if(kn->status == PKG_BD_RECVING){
        int currenlen = strlen(kn->buf) + n;
        if(currenlen < DATA_LEN){
            memcpy((kn->buf)+strlen(kn->buf), buffer, n);
        }else if(currenlen == DATA_LEN){
            memcpy((kn->buf)+strlen(kn->buf), buffer, n);
            kn->status = PKG_BD_FINISH;
        }else{
            LOG_ACC(ERROR, "不合法数据");
            return;
        }
    }
    if(kn->status == PKG_BD_FINISH){
        datapoll.inMsgQueue(kn);
        threadpoll.call();
    }
    // buffer[n+1] = '\0';
    // LOG_ERR(INFO, "收到数据：%s", buffer);
}