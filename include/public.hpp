#ifndef PUBLIC_H
#define PUBLIC_H

enum MsgType{
    login_msg = 1,
    login_msg_ACK,
    regist_msg,
    regist_msg_ACK,
    one_chat_msg,
    add_friend_msg,

    create_group_msg,
    add_group_msg,
    group_chat_msg,

    login_out_msg
};

#endif