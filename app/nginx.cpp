#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "cia_conf.h"
#include "cia_setproctitle.h"
#include "cia_log.h"
#include "cia_kernal_func.h"
#include "cia_signal.h"
// #include "cia_socket.h"
// #include "cia_threadpoo.h"
#include "cia_global.h"
using namespace std;

// 全局变量
int g_environment = 0; // 环境变量的长度
char **g_os_argv; // 命令行参数 
char *new_environment = NULL; //新申请的存放环境变量的空间，主要用于释放
int process_type = 0;

CSocket socket_ctl;
CThreadPoll threadpoll;  // 线程池
DataPoll datapoll;  // 数据池

void freesource(){
    // cout<<"释放资源"<<endl;
    if(new_environment != NULL) delete []new_environment;
    new_environment = NULL;
    // cout<<"释放啦资源"<<endl;
    // log4cpp::Category::shutdown();
}

int main(int argc, char* argv[]){ 
    
    string conf_file_path("./config/lucia.conf");  // 配置文件的默认地址
    
    int exitcode = 0;

    // ----------- 1. 读取参数配置信息 ---------------
    /*
        --file -f path : 加载path位置处的配置文件
    */
    // 参考链接：https://blog.csdn.net/yockie/article/details/51607003
    int option_index = 0;  
    char optstring[] = "f:";  
    static struct option long_options[] = {  
        {"file", required_argument, NULL, 'f'},
        {0, 0, 0, 0}  
    }; 
    char opt;
    while ( (opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1)  
    {  
        switch (opt)
        {
            case '?':
                /* code */
                cout << "配置项 " << argv[optind - 1] << " 缺少参数!" << endl;
                break;
            case 'f':
                conf_file_path = optarg;
            default:
                break;
        }
            // printf("opt = %c\n", opt);  
            // printf("optarg = %s\n", optarg);  
            // printf("optind = %d\n", optind);  
            // printf("argv[optind - 1] = %s\n",  argv[optind - 1]);  
            // printf("option_index = %d\n", option_index);  
    } ;

    // strcpy(argv[0], "luc");  可以通过这个命令修改运行的COMM信息，但是这样子是不安全的；

    // ----------- 2. 修改环境变量的位置，为修改进程的COMM做准备 ----------- 
    
    g_os_argv = argv;
    cia_init_setprcotitle();

    // ----------- 3. 读取配置文件    ---------------
    // 配置文件的单例模式实例：conf_file_instance
    CConfig* conf_file_instance = CConfig::getInstance();
    //  读取配置文件
    if(!conf_file_instance->load(conf_file_path) || !conf_file_instance->read_mime()){
        cout << "配置文件加载失败，退出！" << endl;
        exit(1);
    }
    //    conf_file_instance->test_showAllitem();  日志文件测试
    // ----------- 4. 设置日志，采用log4cpp ---------------------
    cia_logs_init();   //  日志文件初始化
    LOG_ERR(INFO, "----------重新启动-------------");

    if(cia_init_signals() < 0){
        exitcode = 1;
        goto libexist;
    }
    
    if(socket_ctl.socket_init() == false){  // 设置监听端口
        exitcode = 2; 
        goto libexist;
    }

    // 按照守护进程的方式进行执行
    if(CConfig::getInstance()->GetIntDefault("Daemon") == 1){
        exitcode = cia_daemon();
        if(exitcode != 0){
            goto libexist;
        }
        // cout<<"继续happey" <<endl;
    }
    // 进程开始执行
    cia_master_process_cycle(); 

libexist:
    freesource();
    return exitcode;
}