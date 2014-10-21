#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "ss/socket.h"
#include "netinet/in.h"
#include "client.h"
#include "../proto/cs_pkg.pb-c.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8888
#define BUFSIZE 1000

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
		msg.head_pkg->msg_id = LOGIN_MSG;
		msg.body_pkg->login_pkg->msg_id = PRINT_THE_NAME;
		//strncpy(msg.body_pkg->login_pkg->username);

		//add 4 bytes and put into the buf
		len = cs__pkg__get_packed_size(&msg);
		send_buf = malloc(len + 4);
		cs__pkg__pack(&msg, buf + 4);
		*(int *)send_buf = len;

		//send the pkg
		len = send(client_fd, send_buf, len + 4, 0);
		len = recv(client_fd, recv_buf, BUFSIZE, 0);

		printf("recved %s\n", recv_buf);
	}

	close(client_fd);

	return 0;
}