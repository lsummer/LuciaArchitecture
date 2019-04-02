#ifndef __HEADERS_CIA_COMM_H__
#define __HEADERS_CIA_COMM_H__

#define PKG_UNUSE -1   // 当前节点还在空闲连接池里面呐
#define PKG_INIT 0     // 当前节点从连接池中拿了出来，但是还没有接受数据
#define PKG_BD_RECVING 1  // 已经接受了部分数据，但是还没有完成数据
#define PKG_BD_FINISH 2   // 数据接收完毕

#endif