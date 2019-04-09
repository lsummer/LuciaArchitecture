#ifndef __HEADERS_CIA_RESPONSE_HEADER_H__
#define __HEADERS_CIA_RESPONSE_HEADER_H__

#include <string>

#include <time.h>

#include "cia_func.h"
// HTTP/1.1 200 OK
// Server: nginx
// Date: Mon, 08 Apr 2019 12:02:37 GMT
// Content-Type: text/html; charset=utf-8
// Content-Length: 848
// Last-Modified: Thu, 06 Dec 2018 06:47:45 GMT
// Connection: keep-alive
// ETag: "5c08c611-350"
// Accept-Ranges: bytes

//https://zh.wikipedia.org/wiki/HTTP%E5%A4%B4%E5%AD%97%E6%AE%B5 关于响应报文的wiki
// 先这样写，之后再修正增加；
class Cia_Response_Header{
public:

    virtual std::string getHeader(){  // 可以优化避免copy
        std::string result;
        time_t rawTime; 
        time(&rawTime);

        result = "HTTP/1.1 " + code + " " + code_ds + "\r\n";
        result += ("Server: Lucia/0.0.1 (Unix)\r\n");
        result += ("Date: " +  GetGmtTime(&rawTime) + "\r\n");
        result += ("Content-Type: " + Content_Type  + "\r\n");
        result += ("Last-Modified: " + Last_Modified  + "\r\n");
        result += ("Connection: " + Connection  + "\r\n");
        result += ("Accept-Ranges: none\r\n");
        result += "\r\n";

        return result;
    }

private:
    // std::string HTTP_Version;  //==HTTP/1.1
    std::string code;
    std::string code_ds;
    std::string Content_Type;
    std::string Content_Length;
    std::string Last_Modified;
    std::string Connection;
    // std::string ETag;
    // std::string Accept_Ranges; // == none, 表示不支持范围获取

};
#endif