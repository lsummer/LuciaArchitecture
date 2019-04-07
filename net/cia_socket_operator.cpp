#include "cia_socket.h"
#include <thread>
#include <sys/socket.h>
#include "cia_comm.h"
#include "cia_global.h"

int CSocket::sendProc(Response* res){
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

// 和recv是对应着的
// 返回值 >0表示发送对应大小的数据， 0表示客户端断开， -1 表示缓冲区满， -2表示出错;当收到返回值>0和-1时需要考虑加入到epoll写中，此外-2表示出错，0表示断开连接，析构；
void CSocket::sendResponse(Response* res){
    int n = sendProc(res);
    LOG_ACC(INFO, "------发送数据-------");
    LOG_ACC(INFO, "%s", res->begin);
    // char* msg;  // 指向response，可能需要构造一下response的结构，之后再议
    // char* begin;
    // size_t left_len;
    // std::atomic<int> send_count;
    // int fd; // 发送到文件描述符fd处去

    if(n > 0){  
        if(n == res->left_len ){ // 发送成功
            if(res->send_count > 0){     // 表示加入到epoll中去了，要删掉
                // delete from epoll;
                cia_del_epoll(res->fd, false);
            }
            LOG_ACC(INFO, "------发送完成------");
            delete res;
        }else{
            res->left_len -= n;
            res->begin += n;
            if(res->send_count == 0){
                LOG_ACC(INFO, "----尚未发送完整， 放到epoll中去-----");
                // 加入到epoll中去
                // add epolll
                FD_PORT* fd_port = new FD_PORT(res->fd, false, 0);
                if(cia_add_epoll(fd_port, fd_port->wr_flag?1:0, fd_port->wr_flag?0:1,  &CSocket::cia_wait_responese_handler, res) == false){
                    delete fd_port;
                    LOG_ACC(ERROR, "CSocket::sendResponse() 中向epoll中添加节点时出错[n>0]");
                    return;
                } 
                fd_ports.push_back(fd_port);
            }
            ++res->send_count;
        }

    }else if(n == -1){ // EAGAIN || EWOULDBLOCK,需要加入到epoll中去
        if(res->send_count == 0){
            LOG_ACC(INFO, "----尚未发送完整， 放到epoll中去-----");
            // 加入到epoll中去
            // add epoll
            FD_PORT* fd_port = new FD_PORT(res->fd, false, 0);
            if(cia_add_epoll(fd_port, fd_port->wr_flag?1:0, fd_port->wr_flag?0:1,  &CSocket::cia_wait_responese_handler, res) == false){
                delete fd_port;
                LOG_ACC(ERROR, "CSocket::sendResponse() 中向epoll中添加节点时出错[n==-1]");
                return;
            } 
            fd_ports.push_back(fd_port);
        }
        ++res->send_count;
    }else{  // 表示发送出错或者断开连接 n==0||n==-2，清空response即可
        if(res->send_count > 0){
            cia_del_epoll(res->fd, false);
        }
        delete res; 
    }
}

// recv
void CSocket::porcRequest(Message* message){
    LOG_ACC(INFO,"线程threadid = %d 开始处理消息",std::this_thread::get_id());
    LOG_ACC(INFO,"------- 消息开始 ---------");
    // char buf[11] = "Received!";
    // int n = send(knode->fd, buf, strlen(buf), 0);
    LOG_ACC(INFO, "URL: %s", (message->url).c_str());
    LOG_ACC(INFO, "Headers: ");
    for(auto itre = (message->headers).begin(); itre != (message->headers).end(); itre++){
        LOG_ACC(INFO, "%s:  %s", ((*itre)[0]).c_str(), ((*itre)[1]).c_str());
    }
    LOG_ACC(INFO,"------- 消息结束 ---------");
    // sleep(5);
    LOG_ACC(INFO,"线程threadid = %d 结束处理消息",std::this_thread::get_id());

    int fd = message->fd;
    delete message;

    // 发送数据


    char value[] = "HTTP/1.1 404 Not Found\r\n"
         "Date: Sat, 31 Dec 2005 23:59:59 GMT\r\n"
         "Server: Lucia/0.0.1 (Unix) PHP/5.05\r\n"
         "Content-Type: text/html;charset=utf-8\r\n"
         "Connection: Keep-Alive\r\n"
         "Keep-Alive: timeout=15, max=100\r\n"
         "\r\n"
        "<div class=\"navbar-wrapper\">\n"
        "<div class=\"wgt-navbar\">\n"
        "<div class=\"navbar-bg\">\n"
        "<div class=\"navbar-bg-top\">\n"
        "<div class=\"navbar-content layout\">\n"
        "<div class=\"navbar-content-box\">\n"
        "<dl class=\"index \">\n"
        "<dt><a href=\"/\">首页</a></dt>\n"
        "<dd>\n"
        "<div><a href=\"/calendar/\" target=\"_blank\">历史上的今天</a></div>\n"
        "<div><a href=\"/vbaike/\" target=\"_blank\">百科冷知识</a></div>\n"
        "<div><a href=\"/vbaike#gallary\" target=\"_blank\">图解百科</a></div>\n"
        "</dd>\n"
        "</dl>\n"
        "<dl class=\"cat \">\n"
        "<dt><a>分类</a></dt>\n"
        "<dd>\n"
        "<div><a href=\"/art\" target=\"_blank\">艺术</a></div>\n"
        "<div><a href=\"/science\" target=\"_blank\">科学</a></div>\n"
        "<div><a href=\"/ziran\" target=\"_blank\">自然</a></div>\n"
        "<div><a href=\"/wenhua\" target=\"_blank\">文化</a></div>\n"
        "<div><a href=\"/dili\" target=\"_blank\">地理</a></div>\n"
        "<div><a href=\"/shenghuo\" target=\"_blank\">生活</a></div>\n"
        "<div><a href=\"/shehui\" target=\"_blank\">社会</a></div>\n"
        "<div><a href=\"/renwu\" target=\"_blank\">人物</a></div>\n"
        "<div><a href=\"/jingji\" target=\"_blank\">经济</a></div>\n"
        "<div><a href=\"/tiyu\" target=\"_blank\">体育</a></div>\n"
        "<div><a href=\"/lishi\" target=\"_blank\">历史</a></div>\n"
        "</dd>\n"
        "</dl>\n"
        "<dl class=\"second-know \">\n"
        "<dt><a>秒懂百科</a></dt>\n"
        "<dd><div><a href=\"https://child.baidu.com/\" target=\"_blank\">秒懂少儿APP</a></div>\n"
        "<div><a href=\"/item/秒懂星课堂\" target=\"_blank\">秒懂星课堂</a></div>\n"
        "<div><a href=\"/item/秒懂大师说\" target=\"_blank\">秒懂大师说</a></div>\n"
        "<div><a href=\"/item/秒懂看瓦特\" target=\"_blank\">秒懂看瓦特</a></div>\n"
        "<div><a href=\"/item/秒懂五千年\" target=\"_blank\">秒懂五千年</a></div>\n"
        "<div><a href=\"/item/秒懂全视界\" target=\"_blank\">秒懂全视界</a></div>\n"
        "</dd>\n"
        "</dl>\n"
        "<dl class=\"special \">\n"
        "<dt><a>特色百科</a></dt>\n"
        "<dd><div><a href=\"/museum/\" target=\"_blank\">数字博物馆</a></div>\n"
        "<div><a href=\"/feiyi?fr=dhlfeiyi\" target=\"_blank\">非遗百科</a></div>\n"
        "<div><a href=\"https://shushuo.baidu.com/\" target=\"_blank\">百度数说</a></div>\n"
        "<div><a href=\"/city/\" target=\"_blank\">城市百科</a></div>\n"
        "<div><a href=\"/wikicategory/view?categoryName=恐龙大全\" target=\"_blank\">恐龙百科</a></div>\n"
        "<div><a href=\"/wikicategory/view?categoryName=多肉植物\" target=\"_blank\">多肉百科</a></div>\n"
        "</dd>\n"
        "</dl>\n"
        "<dl class=\"user \">\n"
        "<dt><a>用户</a></dt>\n"
        "<dd>\n"
        "<div><a href=\"/kedou/\" target=\"_blank\">蝌蚪团</a></div>\n"
        "<div><a href=\"/event/ranmeng/\" target=\"_blank\">燃梦计划</a></div>\n"
        "<div><a href=\"/task/\" target=\"_blank\">百科任务</a></div>\n"
        "<div><a href=\"/mall/\" target=\"_blank\">百科商城</a></div>\n"
        "</dd>\n"
        "</dl>\n"
        "<dl class=\"cooperation \">\n"
        "<dt><a>权威合作</a></dt>\n"
        "<dd>\n"
        "<div><a href=\"/operation/cooperation#joint\" target=\"_blank\">合作模式</a></div>\n"
        "<div><a href=\"/operation/cooperation#issue\" target=\"_blank\">常见问题</a></div>\n"
        "<div><a href=\"/operation/cooperation#connection\" target=\"_blank\">联系方式</a></div>\n"
        "</dd>\n"
        "</dl>\n"
        "<div class=\"usercenter\">\n"
        "<div><a href=\"/usercenter\" target=\"_blank\"><em class=\"cmn-icon cmn-icons cmn-icons_navbar-usercenter\"></em>个人中心</a></div>\n"
        "</div></div>\n"
        "</div>\n"
        "</div>\n"
        "</div>\n"
        "</div>\n"
        "</div>\n";
    try{


        char* buf = new char[strlen(value)+2];
        memcpy(buf, value, strlen(value));
        
        Response* response = new Response(buf, strlen(buf));
        response->fd = fd;
        datapoll.inResQueue(response);
    }catch(...){
        LOG_ACC(ERROR, "出现错误");
        return;
    }
    
    
    // int n = send(message->fd, buf, strlen(buf), 0);

    // char buf1[] = "Content-Type: text/html\r\n";
    // n = send(message->fd, buf1, strlen(buf1), 0);

    // char buf2[] = "\r\n";

    // n = send(message->fd, buf2, strlen(buf2), 0);

    // char buf3[] = "Hello World\r\n";
    // n = send(message->fd, buf3, strlen(buf3), 0);

}

