#ifndef __HEADERS_CIA_CONF_H__
#define __HEADERS_CIA_CONF_H__

/* ------------------------------------
// 功能： 加载配置文件的单例类
// 描述： 配置文件解析说明
    // 1. 以 "#" 开头的行作为注释
    // 2. 以 "[" 开头的行作为注释
    // 3. 每一行前后空格会被忽略
    // 4. 空行会被忽略
    // 5. 参数设置格式为: 设置名=参数 
------------------------------------  */

#include "header.h"

class CConfig{
private:
    CConfig();
    CConfig(const CConfig& config);
    CConfig& operator =(const CConfig& confi);

public:
    ~CConfig();

    static CConfig* getInstance();

    
    bool load(const std::string& filename);     // 加载配置文件，true表示加载成功， false表示加载失败

    std::string GetString(const std::string& s);  // 得到配置文件的参数
    int GetIntDefault(const std::string& s);             // 得到配置文件的int参数


    class CConfigHuiShou{                       // 用以回收单例类 
        public:
        ~CConfigHuiShou(){
            if(CConfig::instance != nullptr){
                delete CConfig::instance;
                CConfig::instance = nullptr;
            }
        }
    };

    void test_showAllitem(){
        for(auto itre = conf_item_vector.begin(); itre != conf_item_vector.end(); itre++){
            std::cout << itre->first << ": " << itre->second << std::endl;
        }
    }

private:
    static CConfig* instance;
    std::map<std::string, std::string> conf_item_vector; // 配置项
};

#endif