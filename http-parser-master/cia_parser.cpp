#include <string.h>
#include "cia_parser.h"
#include "cia_log.h"

http_parser_settings CParser::settings;

CParser::CParser(){
    http_parser_init(&parser, HTTP_REQUEST);
    memset(&CParser::settings, 0, sizeof(CParser::settings));
    CParser::settings.on_message_begin = on_message_begin;
    CParser::settings.on_url = on_url;
    CParser::settings.on_header_field = on_header_field;
    CParser::settings.on_header_value = on_header_value;
    CParser::settings.on_headers_complete = on_headers_complete;
    CParser::settings.on_body = on_body;
    CParser::settings.on_message_complete = on_message_complete;

    parser.data = &messagelist;
    // message.type = HTTP_REQUEST;
}

size_t CParser::parser_execute(const char *data, size_t len){
    return http_parser_execute(&parser, &CParser::settings, data, len);
}

void CParser::restore(){
    for(auto itre=messagelist.begin(); itre!=messagelist.end(); itre++){
        if((*itre) != NULL){
            delete (*itre);
            (*itre) = NULL;
        }
    }
    messagelist.clear();
    http_parser_init(&parser, HTTP_REQUEST);
}
CParser::~CParser(){
    http_parser_init(&parser, HTTP_REQUEST);
}

int CParser::on_message_begin(http_parser* _){
    std::list<Message*> *messgae = (std::list<Message*> *)_->data;
    // LOG_ACC(INFO, "\n***MESSAGE BEGIN***\n\n");
    Message* mess = new Message();
    (*messgae).push_back(mess);
    (*messgae).back()->message_begin_cb_called = true;
    return 0;
};

int CParser::on_headers_complete(http_parser* _){
    std::list<Message*> *messgae = (std::list<Message*> *)_->data;
    // LOG_ACC("\n***HEADERS COMPLETE***\n\n");
    (*messgae).back()->headers_complete_cb_called = true;
    return 0;
};
int CParser::on_message_complete(http_parser* _){
    std::list<Message*> *messgae = (std::list<Message*> *)_->data;
    (*messgae).back()->method = _->method;
    // LOG_ACC("\n***MESSAGE COMPLETE***\n\n");
    (*messgae).back()->message_complete_cb_called = true;
    return 0;
};
int CParser::on_url(http_parser* _, const char* at, size_t length){
    std::list<Message*> *messgae = (std::list<Message*> *)_->data;
    // LOG_ACC("Url: %.*s\n", (int)length, at);
    (*messgae).back()->url = std::string(at, at+length);
    return 0;
};
int CParser::on_header_field(http_parser* _, const char* at, size_t length){
    std::list<Message*> *messgae = (std::list<Message*> *)_->data;
    // LOG_ACC("Header field: %.*s\n", (int)length, at);
    std::string header_field = std::string(at,at+length);
    (*messgae).back()->headers.push_back(std::vector<std::string>{header_field});
    (*messgae).back()->header_map[header_field] = (*messgae).back()->headers.size()-1;
    return 0;
};
int CParser::on_header_value(http_parser* _, const char* at, size_t length){
    std::list<Message*> *messgae = (std::list<Message*> *)_->data;
    // LOG_ACC("Header value: %.*s\n", (int)length, at);
    std::string header_v = std::string(at,at+length);
    (*messgae).back()->headers.back().push_back(header_v);
    return 0;
};
int CParser::on_body(http_parser* _, const char* at, size_t length){
    std::list<Message*> *messgae = (std::list<Message*> *)_->data;
    // LOG_ACC(INFO, "on_body接收到的数据大小是：%d", length);
    // LOG_ACC("Body: %.*s\n", (int)length, at);
    (*messgae).back()->body += std::string(at, at+length);
    return 0;
};

   
