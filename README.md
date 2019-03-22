
#### 步骤1.读取配置文件
    - 单例模式(double check 但是未加锁，因为配置文件只在main函数开始时执行)
#### 步骤2.环境变量搬家
    - 因为argv和environ是处于栈底之上，所以如果要修改进程的名称（即COMM），为了避免修改了environ，必须要将环境变量搬家。
#### 步骤3.日志系统
    - 使用了log4cpp 的日志库， 基于 Meyer Singleton的单例模式，log4cpp是线程安全的，并且该库本身所占用内存很少。
    - 使用了可变参数的机制，使用vsprintf进行拼接可变参数。[可变参数的使用方式， va_list , va_start, va_end]
#### 步骤4.进程处理信号
    - 核心使用函数 sigaction(), sigsuspend(), sigprocmask();
#### 步骤5.master进程开启worker进程
    -  fork()， 之后主进程通过sigsuspend()专门等着监听信号，子进程循环执行着
    - 注意设置为守护进程：
        - (1)调用umask(0); umask是个函数，用来限制（屏蔽）一些文件权限的。
	    - (2)fork()一个子进程(脱离终端)出来,然后父进程退出( 把终端空出来，不让终端卡住)；固定套路
	        fork()的目的是想成功调用setsid()来建立新会话，目的是

	        子进程有单独的sid；而且子进程也成为了一个新进程组的组长进程；同时，子进程不关联任何终端了；
	    - (3) 将输入、输出文件描述父重定向到/dev/null；用dup2  
    - 注意避免子进程被杀掉之后成为僵尸进程，需要父进程（master进程 监听信号 SIG_CHLD, 然后waitpid()）
#### 步骤6.worker进程开始端口监听
    socket() -> setsockopt() -> fcntl() -> bind() -> listen()
    设置socket()套接字 -> 设置ipv4,SO_REUSEADDR -> 设置非阻塞 -> 绑定监听端口 -> 监听端口
    ip和端口号要进行htonl(),htons()将本机序转化成网络序实现
    
    epoll LT;
    epoll ET;

#### 步骤7.worker进程开启多线程
    线程池的回收？
#### 步骤8.线程池的分配和处理

#### 步骤9.worker进程处理业务

#### 步骤10.尝试解析http报文

#### 步骤11.静态服务器搭建

#### 步骤12.解析图片
