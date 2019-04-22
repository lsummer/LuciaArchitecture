#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "cia_socket.h"
#include "cia_comm.h"

#include "cia_response_header.h"

bool CSocket::prcoPost(Message* message){
    // for(int i=0; i<message->headers.size(); i++){
    //     LOG_ACC(INFO, "%d+++++%s:%s",i, message->headers[i][0].c_str(), message->headers[i][1].c_str());
    // }
    // for(auto itre = message->header_map.begin(); itre != message->header_map.end(); itre++){
    //     LOG_ACC(INFO, "header_map++++:%s: %d", itre->first.c_str(), itre->second);
    // }
    std::string content_type = message->headers[message->header_map["Content-Type"]][1];

    LOG_ACC(INFO, "contenr_type 的类型值是: %s", content_type.c_str());
    std::string cont;
    int pike = content_type.find_first_of(";");
    if( pike != std::string::npos){
        cont = content_type.substr(0, content_type.find_first_of(";"));
    }else{
        cont = content_type;
    }
    if(cont == "multipart/form-data"){
        std::string boundry = "--" + content_type.substr(content_type.find_last_of("=")+1);
        return parsePostBody(message, message->body, boundry);
    }else if(cont == "application/json"){
        return false;
    }else{
        return false;
    }
     
}


/**
 *  处理Contetn-Type：multipart/form-data 的 POST 数据包体
 * */
bool CSocket::parsePostBody(Message* message, const std::string& body, const std::string& boundry){
    int count = 0, count2 = 0;
    int boundry_length = boundry.length() + 2;  // 因为每一行结尾是有 ”\r\n“的

    int block_end = body.find(boundry, 0);
    int block_begin = block_end + boundry_length; // 因为还有个换行符
    while(block_end != std::string::npos && block_begin < body.length()){
        block_end = body.find(boundry, block_begin);
        if(block_end == std::string::npos){
            break;
        }else{
            // block = body.substr(block_begin, block_end - block_begin);
            // 此处 body中的每一块（称为block的位置为 [block_begin,block_end）)
            // 整理可以针对每一个block进行处理
            if(!parsePostBlock(message, body, block_begin, block_end)){
                return false;
            }
            
            block_begin = block_end + boundry_length; // 因为还有个换行符
        }
    }
    return true;
}

// 输入参数表示HTTPbody值和Block位置的[begin, end)
bool CSocket::parsePostBlock(Message* message, const std::string& body, int begin, int end){
    /**
     * body_header 为每个block 的消息头部描述信息的指针
     * body_body 为每个block的消息内容的指针
     * body_header_l 为 body_header 的长度
     * body_bodt_l 为 body_body 的长度
     * */
    // std::map<string, string> post_body; // 存结果,这里好像似乎应该存成一个全局的值 ？？

    const char* body_header = body.c_str() + begin;  
    const char* body_body;

    int body_header_l = 0, body_body_l = 0;

    int body_header_end_index = body.find("\r\n\r\n", begin);

    if(body_header_end_index != std::string::npos){
        
        body_header_l = body_header_end_index - begin;
        body_body = body.c_str() + body_header_end_index + 4;
        body_body_l = end - (body_header_end_index + 4);

        std::string name, filename;
        procBodyHeader(body_header, body_header_l, name, filename);

        if(filename.length() > 0){  // 说明是file
            FILE* png_file = fopen(filename.c_str(), "w");
            if(png_file == NULL){
                LOG_ACC(ERROR, "CSocket::parsePostBlock() 中 文件filename = %s 打开失败！", filename.c_str());
                return false;
            }else{
                int write_n = fwrite(body_body, sizeof(unsigned char), body_body_l, png_file);
                if(write_n == body_body_l){
                    LOG_ACC(INFO, "CSocket::parsePostBlock() 中 文件filename = %s 写成功！", filename.c_str());
                }else{
                    LOG_ACC(INFO, "CSocket::parsePostBlock() 中 文件filename = %s 没写完整！", filename.c_str());
                    if(fclose(png_file) == EOF){
                        LOG_ACC(INFO, "CSocket::parsePostBlock() 中关闭 文件filename = %s 时出现错误", filename.c_str());
                    }
                    return false;
                }
            }
            if(fclose(png_file) == EOF){
                LOG_ACC(INFO, "CSocket::parsePostBlock() 中关闭 文件filename = %s 时出现错误", filename.c_str());
            }
            
            (message->post_body)[name] = filename;
            // post_body[name] = filename;
        }else{ // 说明不是file
            (message->post_body)[name] = std::string(body_body, body_body_l);
        }

        // LOG_ACC(INFO, "打印message->post_body[%s] = %s", name.c_str(), filename.c_str());
        // FILE* png_file = fopen("./arch.png", "w");
        // if(png_file == NULL){
        //     return false;
        // }else{
        //     int write_n = fwrite(body_body, sizeof(unsigned char), body_body_l, png_file);
        //     if(write_n == body_body_l){
        //         LOG_ACC(INFO, "写成了");
        //     }else{
        //         LOG_ACC(INFO, "没写成");
        //     }
        // }
        // if(fclose(png_file) == EOF){
        //     LOG_ACC(INFO, "关闭文件时出现错误");
        // }
        return true;
    }
    return false;
}

void CSocket::procBodyHeader(const char* body_header, int body_header_l, std::string& name, std::string& filename){
    // 示例值：
    // Content-Disposition: form-data; name="anothers"; filename="Arch.png"
    // Content-Type: image/png 

    // Content-Disposition: form-data; name="anothers"
    std::string header(body_header, body_header_l);

    LOG_ACC(DEBUG, "header信息:%s", header.c_str());
    int name_begin = header.find("name=\"") + 6;  
    int name_end = header.find("\"", name_begin);
    if(name_begin != std::string::npos){
        if(name_end != std::string::npos){
            name = header.substr(name_begin, name_end-name_begin);
        }else{
            name = header.substr(name_begin);
        }
    }else{
        name = "";
        filename = "";
        return;
    }

    LOG_ACC(INFO, "name = %s", name.c_str());

    int file_name_begin = header.find("filename=\"", name_begin);
    if(file_name_begin != std::string::npos){
        file_name_begin += 10;
        int file_name_end = header.find('"', file_name_begin);
        if(file_name_end != std::string::npos){
            filename = header.substr(file_name_begin, file_name_end - file_name_begin);
        }else{
            filename = header.substr(file_name_begin);
        }
    }else{
        filename = "";   
    }
    LOG_ACC(INFO, "filename = %s", filename.c_str());

}