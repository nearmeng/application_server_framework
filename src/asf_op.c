#include "asf_op.h"

int deal_login_msg(CsPkg* msg, client_t* client)
{
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

    return 0;
}

int deal_loc_report_msg(CsPkg* msg, client_t* client)
{
    return 0;
}
