#ifndef __TOOLS_CIA_CONF_CPP__
#define __TOOLS_CIA_CONF_CPP__

#include <regex>  // https://zh.cppreference.com/w/cpp/regex/regex_match
#include "header.h"
#include "cia_conf.h"
#include "cia_func.h"
#include "cia_log.h"
// 静态成员赋初值
CConfig* CConfig::instance = nullptr;

CConfig::CConfig(){}  // 默认构造函数
CConfig::CConfig(const CConfig& config){}  // 拷贝构造函数
CConfig::~CConfig(){                        // 析构函数, 清楚掉每一个配置项的信息
    for(auto itre = static_map.begin(); itre != static_map.end(); itre++){
        if((*itre) != NULL){
            delete (*itre);
            (*itre) = NULL;
        }
    }
    static_map.clear();
    static_map.shrink_to_fit();
    conf_item_vector.clear();
}                      
CConfig& CConfig::operator= (const CConfig& config){  // 赋值构造函数
    return *this;
}

CConfig* CConfig::getInstance(){
    if( instance == nullptr ){
        // 加锁
        if( instance == nullptr){
            instance = new CConfig();
        }
        // 解锁
    }
    return instance;
}
std::string CConfig::GetMime(const std::string& s){
    if(mime.find(s) != mime.end()){
        return mime[s];
    }else{
        return "text/plain";
    }
}

bool CConfig::read_mime(){
    std::string path = "./config/mime.types";
    std::ifstream file(path);

    if(file){
        std::string line;
        while(getline(file, line)){
            if(line.length() > 0){
                LRtrim(line);
                if(line.length() <= 0) continue;
                if(!read_mime_line(line)){
                    return false;
                }
            }
        }
    }
    return true;
}

bool CConfig::read_mime_line(std::string& str){
    int begin = 0;
    std::vector<std::string> vec;
    bool flag = true;
    for(int i=0; i<str.length(); i++){
        if(str[i] == ' ' && flag){
            vec.push_back(str.substr(begin, i-begin));
            flag = false;
        }else if(str[i] != ' ' && !flag){
            begin = i;
            flag = true;
        }
    }
    if(flag){
        vec.push_back(str.substr(begin, str.length()-begin));
    }
    int sz = vec.size();
    if(sz <= 1) return false;

    for(int i=1; i<sz; i++){
        mime[vec[i]] = vec[0];
    }
    return true;
}

bool CConfig::load(const std::string& filename){
    // 每次load之前 清空conf_item_vector 的内容，避免多次load
    conf_item_vector.clear();
    
    std::ifstream file(filename);
    if(file){
        std::string line;
        while(getline(file, line)){             // 读取文件中的每一行
            if(line.length() > 0){           
                LRtrim(line);                   // 去除前后空格
                
                // 判断是否是空格，判断是否是注释行
                if(line.length() <= 0) continue;
                if(line[0] == '#' || line[0] == '[' || line[0] == '\t' || line[0] == '\n' || line[0] == '{' || line[0] == '}') continue;
                
                // 判断location， 静态资源映射
                if(line.length() > 8 && line.substr(0, 8) == "location"){
                    if(ParseLocation(line)){
                        continue;
                    }else{
                        file.close();
                        return false;
                    }
                }

                // 判断等号赋值
                std::string::size_type index = line.find_first_of("=");
                if(index != std::string::npos){
                    
                    std::string iteamname = line.substr(0, int(index));
                    std::string iteamvalue = line.substr(int(index)+1);
                    EraseAnnoation(iteamname);
                    EraseAnnoation(iteamvalue);
                    LRtrim(iteamname);
                    LRtrim(iteamvalue);

                    if(iteamname.length() > 0 && iteamvalue.length() > 0){
                        conf_item_vector[iteamname] = iteamvalue;
                    }
                }
            }
        }

        file.close();
        return true;
    }
    return false;
}

std::string CConfig::GetString(const std::string& s){
    if(conf_item_vector.find(s) != conf_item_vector.end()){
        return conf_item_vector[s];
    }
    
    return "";
}

int CConfig::GetIntDefault(const std::string& s){
    std::string res = GetString(s);
    if(res.empty()){
        return -1;
    }
    return std::stoi(res);
}

bool CConfig::ParseLocation(const std::string& str){
    int begin = 0;
    std::vector<std::string> vec;
    bool flag = true;
    for(int i=0; i<str.length(); i++){
        if(str[i] == ' ' && flag){
            vec.push_back(str.substr(begin, i-begin));
            flag = false;
        }else if(str[i] != ' ' && !flag){
            begin = i;
            flag = true;
        }
    }
    if(flag){
        vec.push_back(str.substr(begin, str.length()-begin));
    }
    CTriple* ctriple = new CTriple();

    if(vec[1] == "~*"){
        if(vec.size() != 4){
            return false;
        }
        ctriple->rule = RE;
        ctriple->pattern = vec[2];
        ctriple->path = vec[3];
    }else if(vec[1] == "="){
        if(vec.size() != 4){
            return false;
        }
        ctriple->rule = EQ;
        ctriple->pattern = vec[2];
        ctriple->path = vec[3];
    }else if(vec[1] == "/"){
        if(vec.size() != 3){
            return false;
        }
        ctriple->rule = OT;
        // ctriple.pattern = vec[2];
        ctriple->path = vec[2];
    }else{
        return false;
    }
    
    static_map.push_back(ctriple);
    
    return true;
}


std::string CConfig::GetPath(const std::string& s){
    int i = 0;
    for(; i<static_map.size(); i++){
        if(match(s, static_map[i])){
            break;
        }
    }
    std::string val = static_map[i]->path + s;
    return static_map[i]->path + s;
}

bool CConfig::match(const std::string& s, const CTriple* triple){

    std::regex txt_regex(triple->pattern, std::regex_constants::icase);

    switch (triple->rule)
    {
        case RE:
            /* code */
           
            return std::regex_search(s, txt_regex);
            break;

        case EQ:
            /* code */
            if(s == triple->pattern) return true;
            return false;
            break;

        default:  // OT
            return true;
            break;
    }
}
#endif
