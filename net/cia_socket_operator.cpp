#include <thread>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <unistd.h>
#include <iostream>
#include "cia_socket.h"
#include "cia_comm.h"

#include "cia_response_header.h"

int CSocket::sendBuf(Response* res){
    int n;
    for(;;){
        
        n = send(res->fd, res->begin, res->left_len, 0);
        
        if(n > 0){
            return n;
        }

        if(n == 0){  // 表示客户端主动断开连接
            return 0;
        }

        int err = errno;

        if(err == EAGAIN || err == EWOULDBLOCK){
            return -1;
        }
        if(err == EINTR){
            ;
        }else{
            return -2;
        }
    }
}
// 关闭fd文件，并将其从res中取出
void CSocket::closeFile(Response* res, int fd){
    if(fd <= 0) return;

    if(close(fd) == -1){
        LOG_ACC(ERROR, "CSocket::sendFile()函数在关闭文件时出现错误");
    }
    res->file_list.pop_front();
}
// 返回值：n
// n > 0 表示成功发送多少数据；
// n == 0 表示客户端关闭连接
// n == -1 表示EAGAIN或者EWOULDBLOCK，发送缓冲区满，等会再试
// n == -2 表示发送出现错误
// n == -3 表示上一个文件发送完成了，继续发送下一个文件
int CSocket::sendFile(Response* res){
    int fn;
    int NBYTES = 2048;
    
    for(;;){
        char* buf = new char[NBYTES];  // delete []buf;
        memset(buf, 0, NBYTES);
        int file = (res->file_list).front();
        fn = read(file, (void *)buf, NBYTES);
        if(fn == 0){
            //表示该文件已经读取到末尾，无需再继续读取了;close文件, 并弹出
            delete []buf;
            closeFile(res, file);
            return -3; // 表示该文件已经发送完毕了，通知sendProc处理下一个文件
        }else if(fn == -1){ // read文件可能出现的错误
            int err = errno;
            
            switch (err)
            {
                case EINTR:
                    //被信号打断，不用管，继续，因为会走for循环，重新读取
                    break;
                case EAGAIN: 
                    // 当前没有数据可读，等会再试
                    delete []buf;
                    return -1;
                    break;
                case EIO:
                    LOG_ACC(ERROR, "CSocket::sendFile()函数read文件时遇到EIO错误");
                    delete []buf;
                    closeFile(res, file);
                    return -2;
                    break;
                case EISDIR:
                    LOG_ACC(ERROR, "CSocket::sendFile()函数read文件时遇到EISDIR错误,读取的文件是一个目录");
                    delete []buf;
                    closeFile(res, file);
                    return -2;
                    break;
                case EBADF:
                    LOG_ACC(ERROR, "CSocket::sendFile()函数read文件时遇到EBADF错误,读取的文件不是一个合法文件");
                    delete []buf;
                    closeFile(res, file);
                    return -2;
                    break;
                case EINVAL:
                    LOG_ACC(ERROR, "CSocket::sendFile()函数read文件时遇到EINVAL错误,文件不可读");
                    delete []buf;
                    closeFile(res, file);
                    return -2;
                    break;
                default:
                    LOG_ACC(ERROR, "CSocket::sendFile()函数read文件时遇到未知错误：errno=%d,读取文件出错", err);
                    delete []buf;
                    closeFile(res, file);
                    return -2;
                    break;
            }
        }else{
            res->msg = buf;
            res->begin = buf;
            res->left_len = fn;
            return sendBuf(res);
        }
    }
}
// 返回值：n
// n > 0 表示成功发送多少数据；
// n == 0 表示客户端关闭连接
// n == -1 表示EAGAIN或者EWOULDBLOCK，发送缓冲区满，等会再试
// n == -2 表示发送出现错误
// n == -3 表示文件已经全部发送完了, 发送结束
int CSocket::sendProc(Response* res){
    int n;
    for(;;){
        // 判断是否发送完成缓冲区文件
        if(!res->msg_finished){
            return sendBuf(res);
        }else{
            if(res->file_list.empty()){
                return -3;
            }
            n = sendFile(res);
            if(n == -3){
                continue;
            }
            return n;
        }     
    }
}

// 和recv是对应着的
// 返回值 >0表示发送对应大小的数据， 0表示客户端断开， -1 表示缓冲区满， -2表示出错;当收到返回值>0和-1时需要考虑加入到epoll写中，此外-2表示出错，0表示断开连接，析构；
void CSocket::sendResponse(Response* res){
    // LOG_ACC(INFO, "------发送数据-------");
    for(;;){
        int n = sendProc(res);
        if(n > 0){
            if(n >= res->left_len){ //msg中的数据发完了
                res->msg_finished = true;
                // delete [](res->msg);
                res->msg = NULL;
                res->begin = NULL;
                res->left_len = -1;
                // LOG_ACC(INFO, "走到这：下一步");
                continue;
            }else{
                res->left_len -= n;
                res->begin += n;
                if(res->send_count == 0){
                    LOG_ACC(INFO, "----尚未发送完整， 放到epoll中去-----");
                    // 加入到epoll中去
                    // add epolll
                    FD_PORT* fd_port = new FD_PORT(res->fd, false, 0);
                    if(cia_add_epoll(fd_port, fd_port->wr_flag?1:0, fd_port->wr_flag?0:1,  &CSocket::cia_wait_responese_handler, res, EPOLL_CTL_MOD) == false){
                        if(fd_port != NULL) delete fd_port;
                        LOG_ACC(ERROR, "CSocket::sendResponse() 中向epoll中添加节点时出错[n>0]");
                        return;
                    } 
                    if(fd_port != NULL) fd_ports.push_back(fd_port);
                }
                ++res->send_count;
            }
        }else if(n == -1){
            if(res->send_count == 0){
                LOG_ACC(INFO, "----尚未发送完整， 放到epoll中去-----");
                // 加入到epoll中去
                // add epoll
                FD_PORT* fd_port = new FD_PORT(res->fd, false, 0);
                if(cia_add_epoll(fd_port, fd_port->wr_flag?1:0, fd_port->wr_flag?0:1,  &CSocket::cia_wait_responese_handler, res, EPOLL_CTL_MOD) == false){
                    if(fd_port != NULL) delete fd_port;
                    LOG_ACC(ERROR, "CSocket::sendResponse() 中向epoll中添加节点时出错[n==-1]");
                    return;
                } 
                if(fd_port != NULL) fd_ports.push_back(fd_port);
            }
            ++res->send_count;
        }else if(n == -3){
            if(res->send_count > 0){     // 表示加入到epoll中去了，要删掉
                // delete from epoll;
                cia_del_epoll(res->fd, false);
            }
            // LOG_ACC(INFO, "------发送完成------");
            delete res;
        }else{
            if(res->send_count > 0){
                cia_del_epoll(res->fd, false);
            }
            delete res; 
        }
        break;
    }
}

// recv
void CSocket::porcRequest(Message* message){
    // LOG_ACC(INFO,"线程threadid = %d 开始处理消息",std::this_thread::get_id());

    int method = message->method;  //GET or POST or OTHERS

    ConstructResponse(message);

    delete message;
}

