#include <thread>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include "cia_socket.h"
#include "cia_global.h"
// 根据请求的url地址，构造 response 的返回格式 content-type
std::string CSocket::GetContentType(const std::string& url){
    std::string suffixStr = url.substr(url.find_last_of('.') + 1);//获取文件后缀
    
    transform(suffixStr.begin(), suffixStr.end(), suffixStr.begin(), ::tolower);
    // if(suffixStr == "html"){
    //     return "text/html;charset=utf-8";
    // }else if(suffixStr == "png"){
    //     return "image/png; charset=utf-8";
    // }else if(suffixStr == "gif"){
    //     return "image/gif; charset=utf-8";
    // }else if(suffixStr == "jpg" || suffixStr == "jpeg" ){
    //     return "image/jpeg; charset=utf-8";
    // }else if(suffixStr == "css"){
    //     return "text/css; charset=UTF-8";
    // }else if(suffixStr == "js"){
    //     return "application/javascript; charset=UTF-8";
    // }
    return CConfig::getInstance()->GetMime(suffixStr);
    // return "";
}

// 通过request header 来构造response header 的部分内容
void CSocket::getRequestHeader(Message* message, Cia_Response_Header* header){
    std::string UserAgent;   // 用户信息
    std::string Connection;  // 连接状态
    std::string IfModifiedSince;  // 改变？

    for(auto itre = (message->headers).begin(); itre != (message->headers).end(); itre++){
        if((*itre)[0] == "User-Agent"){
            UserAgent = (*itre)[1];
        }else if((*itre)[0] == "Connection"){
            Connection = (*itre)[1];
        }else if((*itre)[0] == "If-Modified-Since"){
            IfModifiedSince = (*itre)[1];
        }
    }
    std::string log_access = message->url + "   " + UserAgent;
    LOG_ACC(INFO, log_access.c_str());

    if(Connection.length() > 0){
        header->Connection = Connection;
    }else{
        header->Connection = "close";
    }

    if(header->Last_Modified == IfModifiedSince){
        header->code = "304";
        header->code_ds = "Not Modified";
        header->Content_Length = "0";
    }else{
        header->code = "200";
        header->code_ds = "OK";
        header->Content_Type = GetContentType(message->url);
    }
}

// 根据静态资源构造响应头; 
// 如果资源存在并正确打开，则返回文件句柄；否则返回404.html的文件句柄
int CSocket::GetFile(Message* message, Cia_Response_Header* header){ 
    // 根据请求的url和location得到静态资源地址
    std::string path = CConfig::getInstance()->GetPath(message->url); 
    struct stat buf;

    int filedesc = open(path.c_str(), O_RDONLY);
    if(stat(path.c_str(), &buf) == -1 || filedesc == -1){
        int err = errno;
        LOG_ACC(INFO, "打开文件path = %s 出现错误，errno = %d", path.c_str(), err);
        header->code = "404";
        header->code_ds = "Not Found";   // 这里要返回404的html

        path = CConfig::getInstance()->GetPath("/404.html");
        if(filedesc != -1) close(filedesc);
        filedesc = open(path.c_str(), O_RDONLY);
        if(stat(path.c_str(), &buf) == -1 || filedesc == -1){
            LOG_ACC(INFO, "打开404.html文件出现错误，errno = %d", path.c_str(), err);
            if(filedesc != -1) close(filedesc);
            return -1;
        }
        header->Content_Type = GetContentType("/404.html");
        header->Content_Length = std::to_string(buf.st_size);
        header->Last_Modified = GetGmtTime(&(buf.st_mtime));
    }else{
        header->Last_Modified = GetGmtTime(&(buf.st_mtime));
        header->Content_Length = std::to_string(buf.st_size);
        getRequestHeader(message, header); 
    }      
    
    return filedesc;
}

void CSocket::ConstructResponse(Message* message){
    Cia_Response_Header header;

    int filedesc = GetFile(message, &header);

    if(filedesc != -1){
        if(header.code == "304"){
            close(filedesc);
            filedesc = -1;
        }
    }

    std::string value = header.getHeader();
    char* buffer = new char[value.length() + 2];
    strcpy(buffer, value.c_str()); 

    Response* response = new Response(buffer, strlen(buffer));

    int fd = message->fd;
    response->fd = fd;

    if(filedesc != -1){
        response->file_list.push_back(filedesc);
    }
    
    datapoll.inResQueue(response);
}