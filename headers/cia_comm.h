#ifndef __HEADERS_CIA_COMM_H__
#define __HEADERS_CIA_COMM_H__
#include "http_parser.h"
#include <string>
#include <map>
#include <vector>
#define PKG_UNUSE -1   // 当前节点还在空闲连接池里面呐
#define PKG_INIT 0     // 当前节点从连接池中拿了出来，但是还没有接受数据
#define PKG_BD_RECVING 1  // 已经接受了部分数据，但是还没有完成数据
#define PKG_BD_FINISH 2   // 数据接收完毕


#define MAX_ELEMENT_SIZE 2048
#define MAX_HEADERS 13
class Message {  // 目前message只是处理了request的数据，reponse的数据需要额外处理，需要的时候另说
    public:
    Message():message_begin_cb_called(false),headers_complete_cb_called(false),message_complete_cb_called(false){}

    void clear(){  
        message_begin_cb_called = false;
        headers_complete_cb_called = false;
        message_complete_cb_called = false;
        fd = 0;
        url.clear();
        body.clear();
        headers.clear();
        headers.shrink_to_fit();
        header_map.clear();
    }
    enum http_parser_type type; // HTTP_REQUEST HTTP_REPONSE
    int method;  // 1-GET 3-POST

    int fd;  // 那个文件描述符的消息
    std::string url;
    std::string body;
    std::vector<std::vector<std::string>> headers;
    std::map<std::string, int> header_map;

    bool message_begin_cb_called;
    bool headers_complete_cb_called;
    bool message_complete_cb_called;
};
#endif