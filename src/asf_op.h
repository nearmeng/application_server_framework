#ifndef _ASF_OP_H_
#define _ASF_OP_H_

#include "server.h"


//登陆包处理
int deal_login_msg(CsPkg* msg, client_t* client);

//汇报信息处理
int deal_loc_report_msg(CsPkg* msg, client_t* client);

#endif
