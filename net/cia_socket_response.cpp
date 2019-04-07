
#include "cia_socket.h"

void CSocket::cia_wait_responese_handler(Kevent_Node* kn){
    LOG_ACC(INFO, "开始写数据了----%s", kn->c_response->begin);

    sendResponse(kn->c_response);
    
    return;
}