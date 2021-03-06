/*
    字符串操作的一些函数
    时间格式转换的一些函数
*/

#include "header.h"
#include "cia_func.h"
#include <string.h>
// 去掉左侧的空格
void Ltrim(std::string& s){
    s.erase(s.begin(), find_if(s.begin(), s.end(),[](const char a){return a!=' ';} ) );
}

// 去掉右侧的空格
void Rtrim(std::string& s){
    auto itre = s.rbegin();

    itre = find_if(s.rbegin(), s.rend(), [](const char a){return a != ' ';});
    s.erase(itre.base(), s.end());
}

// 去掉两侧的空格
void LRtrim(std::string& s){
    Ltrim(s);
    Rtrim(s);
}
// 去掉'#'及之后的所有值，'\#'表示'#'
void EraseAnnoation(std::string& s){
    int index = -1;
    for(int i=1; i<s.length(); i++){
        if(s[i-1] != '\\' && s[i] == '#'){
            index = i;
            break;
        }
    }
    if(index > 0){
        s = s.substr(0, index);
    }
}

// ------------ 可变参数拼接操作 ------------------
int args_printf(std::string& result, const char* fmt, va_list args){
    
    char* ch_res = new char[LOGS_SINGLE_MAX_CHARACTERS];
    memset(ch_res, 0, LOGS_SINGLE_MAX_CHARACTERS);
    
    int size = vsprintf(ch_res, fmt, args);
    if(size > 0){
        result = ch_res;
    }
    delete []ch_res;
    ch_res = NULL;
    return size;
}

// ------------ 将时间转换成GMT格式 ----------------
std::string GetGmtTime(const time_t* rawtime){
    struct tm* timeInfo;
    char szTemp[30]={0};
    timeInfo = gmtime(rawtime);
    strftime(szTemp,sizeof(szTemp),"%a, %d %b %Y %H:%M:%S GMT",timeInfo);

    std::string time_str(szTemp, strlen(szTemp));
    return time_str;
}