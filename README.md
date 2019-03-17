
#### 步骤1.读取配置文件
    - 单例模式(double check 但是未加锁，因为配置文件只在main函数开始时执行)
#### 步骤2.环境变量搬家
    - 因为argv和environ是处于栈底之上，所以如果要修改进程的名称（即COMM），为了避免修改了environ，必须要将环境变量搬家。
#### 步骤3.日志系统
    - 使用了log4cpp 的日志库， 基于 Meyer Singleton的单例模式，log4cpp是线程安全的，并且本身该库所占用内存很少。