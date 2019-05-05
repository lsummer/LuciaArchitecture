## Lucia Architecture

`Lucia Server`是一个支持Linux和MacOS的轻量级的静态资源服务器。基于`C++11`，`多进程（1个master进程+多个worker进程）+多线程（Thread Pool）`的架构方式，`epoll(LT)+kqueue的reactor模式`的多路复用技术，完成了在HTTP1.1协议下的对静态资源的请求与响应。主要包含以下几个模块：

```
- 读取配置文件模块
- 基于log4cpp的日志模块
- 信号处理模块
- 进程管理模块
- 线程池模块
- 数据池模块
- http解析和响应模块
```



- [Demo演示地址](http://icontinua.cn:8011/index.html) ：项目的Gitstats。

- [代码流程 (Real README)](https://github.com/lsummer/LuciaArchitecture/blob/master/README_old.md)

## 系统微架构

![Atchitecture](https://github.com/lsummer/LuciaArchitecture/raw/master/resource/git/Arch.png)

## 开始使用

Lucia Server 可以运行在`linux`和`macOS`环境下，已在`Ubuntu 18.04.1 LTS`，`Ubuntu 14.04.5 LTS`和`macOS High Sierra v10.13.3`下测试通过。

#### 下载

`git clone https://github.com/lsummer/LuciaArchitecture`

#### 安装库

[log4cpp](http://log4cpp.sourceforge.net/)
安装完成后需要将include和lib位置放入到.bashrc中 
```
# 临时执行
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/Cellar/log4cpp/1.1.3/include
export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/Cellar/log4cpp/1.1.3/lib
```

#### 运行

```
cd build
cmake ..
make
./bin/lucia
```

#### 命令

```
# 仅供推荐使用

# 查看进程信息
MacOS:  ps -eo pid,ppid,tty,pgid,stat,comm |grep -E 'PID|lucia|zsh'
Linux:  ps -eo pid,ppid,tty,sid,stat,command |grep -E 'PID|lucia'

# 结束进程
sudo kill -9 -[pgid|sid]

# 查看日志
tail -f -n100 ./Log/error.log     # 系统启动运行日志
tail -f -n100 ./Log/access.log    # 业务处理（http请求）日志

# 追踪进程系统调用
MacOS: sudo dtruss -p [pid]
Linux: sudo strace -p [pid]

# GDB 真正debug有用的命令
gdb
```



#### 配置

配置文件地址：`./config`， 主要包含两个配置文件：`log4cpp.properties`和`lucia.conf`，其中`log4cpp.properties`为日志系统的配置文件，`lucia.conf`为服务器启动的相关配置文件。

- `log4cpp.properties`的编写规则见 [链接](https://www.ibm.com/developerworks/cn/linux/l-log4cpp/index.html)，推荐使用默认配置

- `lucia.conf`的编写规则如下：

```
# // 描述： 配置文件解析说明
    # // 1. 以 "#" 开头的行作为注释
    # // 2. 以 "[" 开头的行作为注释
    # // 3. 以 "{" 开头的行作为注释
    # // 4. 以 "}" 开头的行作为注释
    # // 5. 每一行前后空格会被忽略
    # // 6. 空行会被忽略
    # // 7. 参数设置格式为: 设置名=参数 
    # // 8. 参数行后面的 "#" 之后的作为注释
    # // 9. '\#' 代表 #
    # // 10. location 开头的表示一个静态资源映射地址
    # //     10.1 ~* 执行正则匹配，但不区分大小写 （四元组） location ~* pattern path
    # //     10.2 = 执行完全匹配 （四元组） location = pattern path
    # //     10.3 / 所有匹配  （三元组） location / path
```



## 备注

#### 待解决的问题：

```
- 进程重启，datapoll里面的数据可能会丢失掉，所以要有两种重启的方式，暴力重启和温和重启。
- 惊群效应
- 线程资源池的回收再利用需要延迟吗？需要考虑一下
- Http 中的request和response的cookie
```



## 参考文献
- Unix 环境高级编程 (第3版)
- Unix 网络编程卷1：套接字联网API
- [《linux C++通讯架构实战 卷1》 kuangxiang](https://study.163.com/course/courseMain.htm?courseId=1006470001&share=1&shareId=3088415) 良心课程，推荐！
- [kuque学习笔记](http://andrewliu.in/2016/08/14/Kqueue%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/)
- [便利的开发工具-log4cpp快速使用指南](https://www.ibm.com/developerworks/cn/linux/l-log4cpp/index.html)
- [使用gitstats统计本地git代码](https://sealake.net/analyze-your-local-gitrepo-by-gitstats/)
- [深入解析 multipart/form-data](https://www.jianshu.com/p/29e38bcc8a1d)
- [linux 信号列表及分析](https://blog.51cto.com/myunix/1107528)
- [RESTful API 设计指南](http://www.ruanyifeng.com/blog/2014/05/restful_api.html)