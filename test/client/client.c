#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "client.h"
#include "../../proto/cs_pkg.pb-c.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8888
#define BUFSIZE 1000
#define NAME_SIZE 20
#define PWD_SIZE 20

int main()
{
	int ret;
	int client_fd;
	int len;

	struct sockaddr_in remote_addr;
	char recv_buf[BUFSIZE];
	void *send_buf;

	//set the remote addr
	memset(&remote_addr, 0 , sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	remote_addr.sin_port = htons(SERVER_PORT);

	//build the socket
	client_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(client_fd < 0)
	{
		printf("create the socket! \n");
		return -1;
	}

	ret = connect(client_fd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr));
	if(ret < 0)
	{
		printf("connect the server error!\n");
		return -1;
	}

	printf("======connected to the server======\n");

	while(1)
	{
		//create and send a cspkg
		CsPkg msg = CS__PKG__INIT;
        HeadPkg head_pkg = HEAD__PKG__INIT;
        BodyPkg body_pkg = BODY__PKG__INIT;
        LoginPkg login_pkg = LOGIN__PKG__INIT;
        LocReportPkg loc_report_pkg = LOC__REPORT__PKG__INIT;

        msg.head_pkg = &head_pkg;
        msg.body_pkg = &body_pkg;
        msg.body_pkg->login_pkg = &login_pkg;
        msg.body_pkg->login_pkg->username = malloc(NAME_SIZE);
        msg.body_pkg->login_pkg->password = malloc(PWD_SIZE);
        msg.body_pkg->loc_report_pkg = &loc_report_pkg;

		msg.head_pkg->msg_id = LOGIN_MSG;
		msg.body_pkg->login_pkg->msg_id = PRINT_THE_PASSWORD;
		strncpy(msg.body_pkg->login_pkg->username, "nearmeng", 
                sizeof(msg.body_pkg->login_pkg->username));
		strncpy(msg.body_pkg->login_pkg->password, "12345", 
                sizeof(msg.body_pkg->login_pkg->password));

		//add 4 bytes and put into the buf
		len = cs__pkg__get_packed_size(&msg);
		send_buf = (char*)malloc(len + 4);
		cs__pkg__pack(&msg, send_buf + 4);
		*(int *)send_buf = len;
    
        printf("the pkg size is %d\n",len);
		//send the pkg
		len = send(client_fd, send_buf, len + 4, 0);
		len = recv(client_fd, recv_buf, BUFSIZE, 0);

		printf("recved %s\n", recv_buf);
	}

	close(client_fd);

	return 0;
}
