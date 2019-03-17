
#ifndef __HEADERS_CIA_LOG_H__
#define __HEADERS_CIA_LOG_H__
#include "header.h"
#include "cia_func.h"
#include "cia_conf.h"
/**
 * 使用 log4cpp 作为日志打印库
 *      log4cpp有3个主要的组件：categories（类别）、appenders（附加目的地）、和 layouts（布局）。
 *      a) layout类控制输出日志消息的显示样式: log4cpp::BasicLayout log4cpp::PatternLayout log4cpp::SimpleLayout
 *      b) appender类用来输出日志（被layout格式化后的）到一些设备上。比如文件、syslog服务、某个socket等。
 *         appender和layout的关系是layout附在appender上(通过appender.setlayout)，appender类调用layout处理完日志消息后，记录到某个设备上。
 *         log4cpp当前提供以下appender：
 *              log4cpp::IdsaAppender     // // 发送到IDS或者logger, 详细见 http://jade.cs.uct.ac.za/idsa/
 *              log4cpp::FileAppender       // 输出到文件
 *              log4cpp::RollingFileAppender   // 输出到回卷文件，即当文件到达某个大小后回卷
 *              
 *              log4cpp::OstreamAppender     // 输出到一个ostream类
 *              log4cpp::RemoteSyslogAppender   // 输出到远程syslog服务器
 *              log4cpp::StringQueueAppender   // 内存队列
 *              log4cpp::SyslogAppender     // 本地syslog
 *              log4cpp::Win32DebugAppender   // 发送到缺省系统调试器
 *              log4cpp::NTEventLogAppender   // 发送到win 事件日志
 *      c)  categories类真正完成记录日志功能，两个主要组成部分是appenders和priority（优先级）
 *             优先级：NOTSET < DEBUG < INFO < NOTICE < WARN < ERROR < CRIT < ALERT < FATAL = EMERG
 *
 *  
 **/

// 定义两个宏函数, LOG_ACC 为访问日志， LOG_ERR为启动错误日志
// 参数level为log4cpp::Priority::Value 的 值， 取值为NOTSET < DEBUG < INFO < NOTICE < WARN < ERROR < CRIT < ALERT < FATAL = EMERG
#define LOG_ACC(level, format, args...) do {\
        CClog::getLog().access_log(level, format, ##args);\
    }while(0);


#define LOG_ERR(level, format, args...) do {\
        CClog::getLog().error_log(level, format, ##args); \
    }while(0);

class CClog{
private:
    CClog();
    // CClog(const std::string& configname);
    ~CClog();
    void init();
    void cleanup();
public:
    static CClog& getLog();

    void access_log(int priority, const char* info, ...);
    void error_log(int priority, const char* info, ...);
private:
    log4cpp::Category* access;   // 该文件表示业务的日志文件
    log4cpp::Category* error;    // 该文件表示系统的加载运行文件
};

#endif