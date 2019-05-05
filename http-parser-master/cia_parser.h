#ifndef __HEADER_CIA_PARSER_H__
#define __HEADER_CIA_PARSER_H__
#include "http_parser.h"
#include "cia_comm.h"
#include <list>
// 解析器:输入data流，输出message
class CParser{
public:
    CParser();
    ~CParser();
    void restore();                       
    size_t parser_execute(const char *data, size_t len);
    
    std::list<Message*> messagelist;
    http_parser parser;

    static http_parser_settings settings;
    static int on_message_begin(http_parser* _);
    static int on_headers_complete(http_parser* _);
    static int on_message_complete(http_parser* _);
    static int on_url(http_parser* _, const char* at, size_t length);
    static int on_header_field(http_parser* _, const char* at, size_t length);
    static int on_header_value(http_parser* _, const char* at, size_t length);
    static int on_body(http_parser* _, const char* at, size_t length);
};
#endif