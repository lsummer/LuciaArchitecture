// #include <thread>
// #include <sys/socket.h>
// #include "cia_operation.h"
// #include "cia_comm.h"
// // 要重写
// // void porcRequest(Message* message){
//     LOG_ACC(INFO,"线程threadid = %d 开始处理消息",std::this_thread::get_id());
//     LOG_ACC(INFO,"------- 消息开始 ---------");
//     // char buf[11] = "Received!";
//     // int n = send(knode->fd, buf, strlen(buf), 0);
//     LOG_ACC(INFO, "URL: %s", (message->url).c_str());
//     LOG_ACC(INFO, "Headers: ");
//     for(auto itre = (message->headers).begin(); itre != (message->headers).end(); itre++){
//         LOG_ACC(INFO, "%s:  %s", ((*itre)[0]).c_str(), ((*itre)[1]).c_str());
//     }
//     LOG_ACC(INFO,"------- 消息结束 ---------");
//     sleep(5);
//     LOG_ACC(INFO,"线程threadid = %d 结束处理消息",std::this_thread::get_id());

//     delete message;
   
//     // char buf[1024] = "HTTP/1.1 200 OK\r\n";
        
//         //  "Date: Tue, 04 Aug 2009 07:59:32 GMT\r\n"
//         //  "Server: Apache\r\n"
//         //  "X-Powered-By: Servlet/2.5 JSP/2.1\r\n"
//         //  "Content-Type: text/xml; charset=utf-8\r\n"
//         //  "Connection: close\r\n"
//         //  "\r\n";
//         //  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
//         //  "<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
//         //  "  <SOAP-ENV:Body>\n"
//         //  "    <SOAP-ENV:Fault>\n"
//         //  "       <faultcode>SOAP-ENV:Client</faultcode>\n"
//         //  "       <faultstring>Client Error</faultstring>\n"
//         //  "    </SOAP-ENV:Fault>\n"
//         //  "  </SOAP-ENV:Body>\n"
//         //  "</SOAP-ENV:Envelope>";
//     // int n = send(message->fd, buf, strlen(buf), 0);

//     // char buf1[] = "Content-Type: text/html\r\n";
//     // n = send(message->fd, buf1, strlen(buf1), 0);

//     // char buf2[] = "\r\n";

//     // n = send(message->fd, buf2, strlen(buf2), 0);

//     // char buf3[] = "Hello World\r\n";
//     // n = send(message->fd, buf3, strlen(buf3), 0);

// }

