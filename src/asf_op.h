#ifndef _ASF_OP_H_
#define _ASF_OP_H_

#include "server.h"

enum login_res_msg_type
{
	LOGIN_RES_MSG_TYPE_INVALID = 0,
	RES_PRINT_THE_NAME = 1,
	RES_PRINT_THE_PASSWORD = 2,
	RES_LOGIN_MSG_TYPE_MAX
};


//登陆包处理
int deal_login_msg(CsPkg* msg, client_t* client);

//汇报信息处理
int deal_loc_report_msg(CsPkg* msg, client_t* client);

#endif
