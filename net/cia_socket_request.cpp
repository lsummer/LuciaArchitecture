#include "cia_socket.h"
#include "cia_global.h"

#define DATA_LEN 20
static size_t MAX_IOSTREAM_ELEMENT = 2000;
void CSocket::cia_wait_request_handler(Kevent_Node* kn){
    
    
    // LOG_ERR(INFO, "pid = %d 来了数据了来了数据了", getpid());
    char buffer[MAX_IOSTREAM_ELEMENT];
    memset(buffer, 0, MAX_IOSTREAM_ELEMENT);
    int n = recv(kn->fd, buffer, MAX_IOSTREAM_ELEMENT-1, 0);
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

    // if(kn->status == PKG_UNUSE){
    //     LOG_ACC(ERROR, "连接池已经被回收，又被使用");
    //     return;
    // }else if(kn->status == PKG_INIT){  // 只有第一次使用的时候是PKG_INIT，所有后续的操作都需要是PK_INIT的后面的步骤
    //     char *buf =  new char[DATA_LEN+1];
    //     memset(buf, 0, DATA_LEN+1);
    //     kn->buf = buf;
    //     if(n < DATA_LEN){
    //         memcpy(buf, buffer, n);
    //         kn->status = PKG_BD_RECVING;
    //     }else if(n == DATA_LEN){
    //         memcpy(buf, buffer, n);
    //         kn->status = PKG_BD_FINISH;
    //     }else{
    //         LOG_ACC(ERROR, "不合法数据！");
    //         return;
    //     }
    // }else if(kn->status == PKG_BD_RECVING){
    //     int currenlen = strlen(kn->buf) + n;
    //     if(currenlen < DATA_LEN){
    //         memcpy((kn->buf)+strlen(kn->buf), buffer, n);
    //     }else if(currenlen == DATA_LEN){
    //         memcpy((kn->buf)+strlen(kn->buf), buffer, n);
    //         kn->status = PKG_BD_FINISH;
    //     }else{
    //         LOG_ACC(ERROR, "不合法数据");
    //         return;
    //     }
    // }

    // size_t nparsed = http_parser_execute(&parser, &settings, data, file_length);

    // if (nparsed != (size_t)file_length) {
    //     fprintf(stderr,
    //             "Error: %s (%s)\n",
    //             http_errno_description(HTTP_PARSER_ERRNO(&parser)),
    //             http_errno_name(HTTP_PARSER_ERRNO(&parser)));
    //     goto fail;
    // }
    size_t nparsed = kn->c_message->parser_execute(buffer, n);

    if (nparsed != (size_t)n) {
        LOG_ACC(ERROR,
                "Error: %s (%s)\n",
                http_errno_description(HTTP_PARSER_ERRNO(&(kn->c_message->parser))),
                http_errno_name(HTTP_PARSER_ERRNO(&(kn->c_message->parser))));
        return;
    }

    CParser* parser = kn->c_message;

    int i=0;
    while(!(parser->messagelist.empty()) && parser->messagelist.front()->message_complete_cb_called){
        Message* me = parser->messagelist.front();
        parser->messagelist.pop_front();
        me->fd = kn->fd;
        
        datapoll.inMsgQueue(me);
        threadpoll.call();
    }
}