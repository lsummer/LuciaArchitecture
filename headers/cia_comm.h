#ifndef __HEADERS_CIA_COMM_H__
#define __HEADERS_CIA_COMM_H__
#include <string>
#include <map>
#include <list>
#include <vector>
#include "http_parser.h"

#define PKG_UNUSE -1   // 当前节点还在空闲连接池里面呐
#define PKG_INIT 0     // 当前节点从连接池中拿了出来，但是还没有接受数据
#define PKG_BD_RECVING 1  // 已经接受了部分数据，但是还没有完成数据
#define PKG_BD_FINISH 2   // 数据接收完毕


#define MAX_ELEMENT_SIZE 2048
#define MAX_HEADERS 13

// request;
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

// class FileStream{
// public:
//     FileStream():filedesc(0), send_begin(0), file_left_size(-1){}
//     int filedesc;
//     int send_begin;
//     int file_left_size;
// };

class Response{
public:
    ~Response(){
        if(msg != NULL) delete []msg;
        clear();
    }
    Response():msg(NULL), begin(NULL), left_len(-1), send_count(0), msg_finished(false){
    }
    Response(char* s, size_t length):msg(s), begin(s), left_len(length), send_count(0), msg_finished(false){
    }
    void clear(){
        send_count = 0;
        msg = NULL;
        begin = NULL;
        left_len = -1;
        fd = 0;
        msg_finished = false;
        file_list.clear();
    }
    char* msg;  // 指向response，可能需要构造一下response的结构，之后再议
    char* begin;

    size_t left_len; // header头文件相关的信息
    
    bool msg_finished; // msg区域是否发送完成
    std::list<int> file_list;

    std::atomic<int> send_count;
    int fd; // 发送到文件描述符fd处去
};

#endif