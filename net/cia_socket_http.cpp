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
    // 针对restful api 请求
    if(url.find_last_of('.') == std::string::npos) return "application/json;charset=UTF-8";

    std::string suffixStr = url.substr(url.find_last_of('.') + 1);//获取文件后缀
    
    transform(suffixStr.begin(), suffixStr.end(), suffixStr.begin(), ::tolower);
    
    return CConfig::getInstance()->GetMime(suffixStr);

}

// 通过request header 来构造response header 的部分内容
void CSocket::getRequestHeader(Message* message, Cia_Response_Header* header){
    std::string UserAgent;   // 用户信息
    std::string Connection;  // 连接状态
    std::string IfModifiedSince = "no";  // 改变？

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
    if(path.length() == 0){
        LOG_ACC(ERROR, "CSocket::GetFile() 匹配静态资源地址失败，没有匹配到任何静态资源地址。");
        return -1;
    }

    struct stat buf;

    int filedesc = open(path.c_str(), O_RDONLY);
    if(stat(path.c_str(), &buf) == -1 || filedesc == -1){
        int err = errno;
        LOG_ACC(INFO, "打开文件path = %s 出现错误，errno = %d", path.c_str(), err);
        header->code = "404";
        header->code_ds = "Not Found";   // 这里要返回404的html

        path = CConfig::getInstance()->GetPath("/404.html");
        if(path.length() == 0){
            LOG_ACC(ERROR, "CSocket::GetFile() 匹配静态资源404.html 地址失败，没有匹配到任何静态资源地址。");
            return -1;
        }

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

void CSocket::ConstructResponseStatic(Message* message){
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

void CSocket::ConstructResponseRestful(Message* message){
    Cia_Response_Header header;

    // 构造 response的 Code, Code_ds, Content_Type, Connection
    getRequestHeader(message, &header);
    header.Last_Modified = "";

    std::string body = "{ \
            \"msg\": \"Success\", \
            \"code\": 200, \
            \"data\": [ \
                \"请求成功.\"   \
            ] \
        }";
    header.Content_Length = std::to_string(body.length());

    std::string value = header.getHeader();
    char* buffer = new char[value.length() + body.length() + 2];
    strcpy(buffer, value.c_str()); 
    strcpy(buffer+value.length(), body.c_str());

    Response* response = new Response(buffer, strlen(buffer));

    LOG_ERR(DEBUG, "发送的响应数据为:%s", response->begin);
    int fd = message->fd;
    response->fd = fd;

    datapoll.inResQueue(response);
}

void CSocket::ConstructResponseOther(Message* message){
    ConstructResponseStatic(message); // 其他目前也先按照静态资源请求的方式来处理
}

/**
 * 构造response信息。
 * 备注：response有两种，一种是静态资源请求的response信息，另一种是RestFul API请求的response信息。
 *      如何筛选出这两种请求： 通过函数CConfig::getInstance()->RequestType(string& url)
 * */
// 在这里判断 是 静态资源请求 还是 RESTFul Api 请求
void CSocket::ConstructResponse(Message* message){

    int request_type = CConfig::getInstance()->RequestType(message->url); 

    if(request_type == 0){   // 静态资源请求
        ConstructResponseStatic(message);
    }else if(request_type == 1){  // RestFul Api 请求
        ConstructResponseRestful(message);
    }else{
        ConstructResponseOther(message);
    }
    
}