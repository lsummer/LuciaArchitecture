#ifndef __TOOLS_CIA_CONF_CPP__
#define __TOOLS_CIA_CONF_CPP__

#include "header.h"
#include "cia_conf.h"
#include "cia_func.h"

// 静态成员赋初值
CConfig* CConfig::instance = nullptr;

CConfig::CConfig(){}  // 默认构造函数
CConfig::CConfig(const CConfig& config){}  // 拷贝构造函数
CConfig::~CConfig(){                        // 析构函数, 清楚掉每一个配置项的信息
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

            static CConfigHuiShou c1;
        }
        // 解锁
    }
    return instance;
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
                if(line[0] == '#' || line[0] == '[' || line[0] == '\t' || line[0] == '\n') continue;

                std::string::size_type index = line.find_first_of("=");
                if(index != std::string::npos){
                    
                    std::string iteamname = line.substr(0, int(index));
                    std::string iteamvalue = line.substr(int(index)+1);
                    EraseAnnoation(iteamname);
                    EraseAnnoation(iteamvalue);
                    LRtrim(iteamname);
                    LRtrim(iteamvalue);


                    if(iteamname.length() > 0 && iteamvalue.length() > 0){
                        // LPConfItem lpconfitem = new ConfItem();
                        // lpconfitem->iteamName = iteamname;
                        // lpconfitem->iteamValue = iteamvalue;
                        conf_item_vector[iteamname] = iteamvalue;
                        // conf_item_vector.push_back(lpconfitem);
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
    // for(auto itre = conf_item_vector.begin(); itre != conf_item_vector.end(); itre++){
    //     if((*itre)->iteamName == s){
    //         return (*itre)->iteamValue;
    //     }
    // }
    return "";
}

int CConfig::GetIntDefault(const std::string& s){
    std::string res = GetString(s);
    if(res.empty()){
        return -1;
    }
    return std::stoi(res);
}

#endif
