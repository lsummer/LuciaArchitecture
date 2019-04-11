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

// 用到了正则匹配，参考网页：// https://zh.cppreference.com/w/cpp/regex/regex_match

#include "header.h"

enum STATIC_MAP { RE, EQ, OT }; // RE表示不区分大小写的正则匹配「~*」，EQ表示相等「=」， OT表示其他「/」

class CTriple{
public:
    enum STATIC_MAP rule;
    std::string pattern;
    std::string path;
};

class CConfig{
private:
    CConfig();
    CConfig(const CConfig& config);
    CConfig& operator =(const CConfig& confi);
    bool ParseLocation(const std::string& str);
public:
    ~CConfig();

    static CConfig* getInstance();
    
    bool load(const std::string& filename);         // 加载配置文件，true表示加载成功， false表示加载失败

    bool read_mime();
    bool read_mime_line(std::string& str);

    std::string GetString(const std::string& s);    // 得到配置文件的参数
    int GetIntDefault(const std::string& s);        // 得到配置文件的int参数
    std::string GetPath(const std::string& s);      // 得到静态资源的地址，首先根据static_map来得到静态资源的地址，然后拼接地址

    std::string GetMime(const std::string& s);
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

    bool match(const std::string& s, const CTriple* triple);

    static CConfig* instance;
    std::map<std::string, std::string> conf_item_vector; // 配置项
    std::map<std::string, std::string> mime;             // MIME_TYPE
    std::vector<CTriple*> static_map; 
};

#endif