#include "cia_log.h"

CClog::CClog(){
    init();
}

CClog::~CClog(){ 
    cleanup();
}

void CClog::init(){
    // 1 读取解析配置文件, 默认为  "log4cpp.properties"
    // 读取出错, 完全可以忽略，可以定义一个缺省策略或者使用系统缺省策略
    // BasicLayout输出所有优先级日志到ConsoleAppender
    try{
        
        std::string configname = CConfig::getInstance()->GetString("log_properties");
        if(configname.length() <= 0){
            configname = "log4cpp.properties";
        }
        
        log4cpp::PropertyConfigurator::configure(configname);  // 加载配置文件
        
        log4cpp::Category& rot = log4cpp::Category::getRoot();  
        access = &rot;              // 访问日志文件
        log4cpp::Category& eor = log4cpp::Category::getInstance(std::string("err"));
        error = &eor; // 错误日志文件

    }catch(std::exception e){
        std::cout << "加载配置文件出现错误，程序强制退出" << std::endl; 
        exit(-1);  // 直接退出
    }
}
void CClog::cleanup(){
    log4cpp::Category::shutdown();
}

CClog& CClog::getLog(){
    static CClog cclog;
    return cclog; 
}

void CClog::access_log(int priority, const char* info, ...){
    va_list args;
    va_start(args, info);
    // TODO 解析参数
    std::string result;

    if(args_printf(result, info, args) < 0){
        va_end(args);
        return ;
    }
    va_end(args);
    access->log(priority, result);
}
void CClog::error_log(int priority, const char* info, ...){
    va_list args;
    va_start(args, info);
    // TODO 解析参数
    std::string result;

    if(args_printf(result, info, args) < 0){
        va_end(args);
        return ;
    }
    va_end(args);
    
    error->log(priority, result);
}

