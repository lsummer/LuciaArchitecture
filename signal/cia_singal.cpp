// nginx中创建worker子进程
//官方nginx ,一个master进程，创建了多个worker子进程；
// master process ./nginx 
// worker process
//(i)ngx_master_process_cycle()        //创建子进程等一系列动作
//(i)    ngx_setproctitle()            //设置进程标题    
//(i)    ngx_start_worker_processes()  //创建worker子进程   
//(i)        for (i = 0; i < threadnums; i++)   //master进程在走这个循环，来创建若干个子进程
//(i)            ngx_spawn_process(i,"worker process");
//(i)                pid = fork(); //分叉，从原来的一个master进程（一个叉），分成两个叉（原有的master进程，以及一个新fork()出来的worker进程
//(i)                //只有子进程这个分叉才会执行ngx_worker_process_cycle()
//(i)                ngx_worker_process_cycle(inum,pprocname);  //子进程分叉
//(i)                    ngx_worker_process_init();
//(i)                        sigemptyset(&set);  
//(i)                        sigprocmask(SIG_SETMASK, &set, NULL); //允许接收所有信号
//(i)                        ngx_setproctitle(pprocname);          //重新为子进程设置标题为worker process
//(i)                        for ( ;; ) {}. ....                   //子进程开始在这里不断的死循环

//(i)    sigemptyset(&set); 
//(i)    for ( ;; ) {}.                //父进程[master进程]会一直在这里循环

