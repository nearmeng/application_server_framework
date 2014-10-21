#ifndef _CLIENT_H_
#define _CLIENT_H_

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8888
#define BUFSIZE 1000
#define NAME_SIZE 20
#define PWD_SIZE 20

enum msg_type
{
	MSG_TYPE_INVALID = 0,
	LOGIN_MSG = 1,
	LOC_REPORT_MSG = 2,
	MSG_TYPE_MAX
};

enum login_msg_type
{
	LOGIN_MSG_TYPE_INVALID = 0,
	PRINT_THE_NAME = 1,
	PRINT_THE_PASSWORD = 2,
	LOGIN_MSG_TYPE_MAX
};

#endif
