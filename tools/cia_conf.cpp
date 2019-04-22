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
                
                EraseAnnoation(line);

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
                    // EraseAnnoation(iteamname);
                    // EraseAnnoation(iteamvalue);
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
/**
 *  # // 10. location 开头的表示一个静态资源映射地址
    # //     10.1 ~* 执行正则匹配，但不区分大小写 （四元组） location ~* pattern path
    # //     10.2 ~ 执行正则匹配，区分大小写 （四元组） location ~ pattern path
    # //     10.3 = 执行完全匹配 （四元组） location = pattern path
    # //     10.4 / 所有匹配  （三元组） location / path
    # //     10.5 ^~ 前缀匹配 （四元组）location ^~ pattern path
    # // 11. location 开头的表示一个RestFul API
    # //     11.1 ~* 执行正则匹配，但不区分大小写 （五元组） location ~* pattern API func
    # //     11.2 ~ 执行正则匹配，区分大小写 （五元组） location ~ pattern API func
    # //     11.3 = 执行完全匹配 （五元组） location = pattern API func
    # //     11.4 / 所有匹配  （四元组） location / API func
    # //     11.5 ^~ 前缀匹配 （五元组）location ^~ pattern API func
    # //     备注：为了更简洁方便地区分与静态资源服务器的区别，在func前需要加入关键字API，表示这是一个restful api请求
 * enum STATIC_MAP { RE, EQ, OT, PR, REU }; // RE表示不区分大小写的正则匹配「~*」，EQ表示相等「=」， OT表示其他「/」, PR表示「^~」, REU表示「~」 

 * */
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
        if(vec.size() == 4){
            // return false;
            ctriple->rule = RE;
            ctriple->pattern = vec[2];
            ctriple->path = vec[3];
            ctriple->request_type = 0;
        }else if(vec.size() == 5 && vec[3] == "API"){
            ctriple->rule = RE;
            ctriple->pattern = vec[2];
            ctriple->path = vec[4];
            ctriple->request_type = 1;
        }else{
            return false;
        }
        
    }else if(vec[1] == "="){
        if(vec.size() == 4){
            ctriple->rule = EQ;
            ctriple->pattern = vec[2];
            ctriple->path = vec[3];
            ctriple->request_type = 0;
        }else if(vec.size() == 5 && vec[3] == "API"){
            ctriple->rule = EQ;
            ctriple->pattern = vec[2];
            ctriple->path = vec[4];
            ctriple->request_type = 1;
        }else{
            // LOG_ACC(ERROR, "出现错误");
            std::cout << "出现错误" << vec.size() << vec[3] << std::endl;
            return false;
        }
        
    }else if(vec[1] == "/"){
        if(vec.size() == 3){
            ctriple->rule = OT;
            // ctriple.pattern = vec[2];
            ctriple->path = vec[2];
            ctriple->request_type = 0;
        }else if(vec.size() == 4 && vec[2] == "API"){
            ctriple->rule = OT;
            // ctriple->pattern = vec[2];
            ctriple->path = vec[3];
            ctriple->request_type = 1;
        }else{
            return false;
        }
    }else if(vec[1] == "~"){
        if(vec.size() == 4){
            ctriple->pattern = vec[2];
            ctriple->path = vec[3];
            ctriple->rule = REU;
            ctriple->request_type = 0;
        }else if(vec.size() == 5 && vec[3] == "API"){
            ctriple->rule = REU;
            ctriple->pattern = vec[2];
            ctriple->path = vec[4];
            ctriple->request_type = 1;
        }else{
            return false;
        }
    }else if(vec[1] == "^~"){
        if(vec.size() == 4){
            ctriple->rule = PR;
            ctriple->pattern = vec[2];
            ctriple->path = vec[3];
            ctriple->request_type = 0;
        }else if(vec.size() == 5 && vec[3] == "API"){
            ctriple->rule = PR;
            ctriple->pattern = vec[2];
            ctriple->path = vec[4];
            ctriple->request_type = 1;
        }else{
            return false;
        }
    }else{
        return false;
    }
    
    static_map.push_back(ctriple);
    
    return true;
}

int CConfig::RequestType(const std::string& s){
    int i = 0;
    for(; i<static_map.size(); i++){
        if(match(s, static_map[i])){   // 既要匹配又要是静态资源服务器
            break;
        }
    }
    if(i >= static_map.size()){
        return -1;
    }
    // LOG_ERR(DEBUG, "请求 %s的类型为：%d", s.c_str(),static_map[i]->request_type);
    return static_map[i]->request_type;

}

// 静态资源服务器
std::string CConfig::GetPath(const std::string& s){
    int i = 0;
    for(; i<static_map.size(); i++){
        if(match(s, static_map[i]) && static_map[i]->request_type == 0){   // 既要匹配又要是静态资源服务器
            break;
        }
    }

    if(i >= static_map.size()){
        return ""; // 表示失败了，没有匹配到任何静态资源地址
    }
    std::string val = static_map[i]->path + s;
    return val;
    // return static_map[i]->path + s;
}

bool CConfig::match(const std::string& s, const CTriple* triple){

    std::regex txt_regex(triple->pattern, std::regex_constants::icase);
    std::regex txt2_regex(triple->pattern);

    switch (triple->rule)
    {
        case RE:
            /* code */
           
            return std::regex_search(s, txt_regex);
            break;
        case REU:
            return std::regex_search(s, txt2_regex);
            break;
        case PR:
            return s.find(triple->pattern) == 0;
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
