#include "cia_setproctitle.h"
#include <iostream>
#include "cia_global.h"

void cia_init_setprcotitle(){
    g_environment = 0;

    for(int i=0; environ[i] != NULL; i++){
        g_environment += (strlen(environ[i]) + 1);
    }
    // for(int i=0; environ[i] != NULL; i++){
    //     std::cout<<environ[i]<<std::endl;
    // }
    char* new_environment = new char[g_environment];

    memset(new_environment, 0, g_environment);

    char *p = new_environment;

    for(int i=0; environ[i] != NULL; i++){
        memcpy(p, environ[i], strlen(environ[i])+1);
        environ[i] = p;
        p = p + 1 + strlen(environ[i]);
    }

    // for(int i=0; environ[i] != NULL; i++){
    //     std::cout<<environ[i];
    // }
    // std::cout<<"-----------------环境变量"<<std::endl;
}

void cia_setproctitle(const std::string& procname){
    

    std::cout << "长度：" << g_environment << std::endl;
    int lenth = procname.length();
    int argv_len = 0;
    for(int i=0; g_os_argv[i] != NULL; i++){
        argv_len += (strlen(g_os_argv[i])+1);
    }

    int havelenth = argv_len + g_environment;
    if(lenth > havelenth){
        std::cout << "名称太长，改名不成功！" << std::endl;
        return;
    }

    // for(int i=0; i<havelenth; i++){
    //     std::cout<<*(g_os_argv[0]+i);
    // }
    // std::cout<<std::endl;

    strcpy(g_os_argv[0], (char*)procname.c_str());
    g_os_argv[1] = NULL;

    char* leave = g_os_argv[0];
    leave += lenth;
    unsigned int toclearlen = havelenth - lenth;
    memset(leave, 0, toclearlen);  //将之后的内存清0

    // std::cout << "修改之后------------------------------" << std::endl;
    // for(int i=0; i<havelenth; i++){
    //     std::cout<<*(g_os_argv[0]+i);
    // }

}