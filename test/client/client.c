#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "client.h"
#include "../../proto/cs_pkg.pb-c.h"


int main()
{
	int ret;
	int client_fd;
	int len;

	struct sockaddr_in remote_addr;
	char recv_buf[BUFSIZE];
	void *send_buf;
	CsPkg *msg_recv;

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

//	while(1)
	{
		//create a cspkg
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

        //填充数据包
		msg.head_pkg->msg_id = LOGIN_MSG;
		msg.body_pkg->login_pkg->msg_id = PRINT_THE_PASSWORD;
		strncpy(msg.body_pkg->login_pkg->username, "nearmeng", 
                sizeof(msg.body_pkg->login_pkg->username));
		strncpy(msg.body_pkg->login_pkg->password, "12345", 
                sizeof(msg.body_pkg->login_pkg->password));

		//在包前插入4个字节用于表征包的长度
		len = cs__pkg__get_packed_size(&msg);
		send_buf = (char*)malloc(len);
		cs__pkg__pack(&msg, send_buf);
    
        printf("the pkg size is %d\n",len);
		//send the pkg
		len = send(client_fd, send_buf, len, 0);
		len = recv(client_fd, recv_buf, BUFSIZE, 0);

		//解析接收到的包
    	msg_recv = cs__pkg__unpack(NULL, len, (uint8_t*) &recv_buf);

    	int recv_id = msg_recv->body_pkg->login_res_pkg->msg_id;
    	char* server_name = msg_recv->body_pkg->login_res_pkg->server_name;
    	int result_code = msg_recv->body_pkg->login_res_pkg->result_code;

		printf("recved id[%d] server_name[%s] result_code[%d]\n", recv_id, server_name, result_code);

		cs__pkg__free_unpacked(msg_recv, NULL);
	}

	close(client_fd);

	return 0;
}
