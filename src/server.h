#ifndef _SERVER_H_
#define _SERVER_H_

enum msg_type
{
	MSG_TYPE_INVALID = 0,
	LOGIN_MSG = 1,
	LOC_REPORT_MSG = 2,
	MSG_TYPE_MAX
};