#include "asf_op.h"
#include "../proto/cs_pkg.pb-c.h"


static char* realloc_nbytes(char* str, int size)
{
    char* c;

    if(str == NULL)
    {
        c = (char*)malloc(size);
    }
    else
    {
        c = (char*)realloc(str, size);
    }

    return c;
}



int deal_login_msg(CsPkg* msg, client_t* client)
{
    int len;
    char username[NAME_SIZE];
    char password[PWD_SIZE];

    strncpy(username, msg->body_pkg->login_pkg->username, sizeof(username));
    strncpy(password, msg->body_pkg->login_pkg->password, sizeof(password));

    switch(msg->body_pkg->login_pkg->msg_id)
    {
        case PRINT_THE_NAME:
            printf("the name is %s\n", username) ;
            break;
        case PRINT_THE_PASSWORD:
            printf("the password is %s\n", password);
            break;

        default:
            printf("wrong msg_id in deal_login_msg\n");
            break;
    }

    //构建返回包
    CsPkg msg_back = CS__PKG__INIT;
    HeadPkg head_pkg = HEAD__PKG__INIT;
    BodyPkg body_pkg = BODY__PKG__INIT;
    LoginResponsePkg login_res_pkg = LOGIN__RESPONSE__PKG__INIT;

    msg_back.head_pkg = &head_pkg;
    msg_back.body_pkg = &body_pkg;
    msg_back.body_pkg->login_res_pkg = &login_res_pkg;
    msg_back.body_pkg->login_res_pkg->server_name = malloc(NAME_SIZE);

    //填充数据包
    msg_back.head_pkg->msg_id = LOGIN_MSG;
    msg_back.body_pkg->login_res_pkg->msg_id = RES_PRINT_THE_PASSWORD;
    strncpy(msg_back.body_pkg->login_res_pkg->server_name, "hx-server", 
                NAME_SIZE);
    msg_back.body_pkg->login_res_pkg->result_code = 0;

    //序列化，按大小分配内存
    len = cs__pkg__get_packed_size(&msg_back);
    client->response = realloc_nbytes(client->response, len);
    if(client->response == NULL)
    {
        printf("memory is not enough\n");
        return -1;
    }
    cs__pkg__pack(&msg_back, (uint8_t*)client->response);
    client->response_len = len;
    free(msg_back.body_pkg->login_res_pkg->server_name);

    return 0;
}

int deal_loc_report_msg(CsPkg* msg, client_t* client)
{
    return 0;
}
