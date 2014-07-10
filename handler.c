#include "handler.h"

/**
* 返回格式
* res = {
*      type:'RES',
*      status:'OK or ERROR'
*      retcode:,
*      description:"",
*      mode:'SERVER',
*      cmd:'ADD_FRIEND',
*      from:'LOCALHOST',
*      remoteID:'',
*   }
*/

void mjson_error(int errcode){
    if(errcode == -1){
        printf("JSON:error");
        exit(EXIT_FAILURE);
    }
}

int mtox_friend_add(Tox *m, const char *friendID, int sock){
    printf("ADD_FRIEND:%s\n",friendID);
    int friend_num = tox_add_friend(m,hex_string_to_bin(friendID),"HELLO",strlen("HELLO"));
    tox_add_friend_norequest(m,friendID);
    json_t *res = json_object();
    int n;
    int err;
    if(friend_num >= 0){
        err = json_object_set(res,"type",json_string("RES"));
        mjson_error(err);
        err = json_object_set(res,"status",json_string("OK"));
        mjson_error(err);
        err = json_object_set(res,"retcode",json_integer(101));
        mjson_error(err);
        err = json_object_set(res,"description",json_string("add friend success"));
        mjson_error(err);
        err = json_object_set(res,"mode",json_string("SERVER"));
        mjson_error(err);
        err = json_object_set(res,"cmd",json_string("ADD_FRIEND"));
        mjson_error(err);
        err = json_object_set(res,"from",json_string("LOCALHOST"));
        mjson_error(err);
        err = json_object_set(res,"remoteID",json_string(friendID));
        mjson_error(err);
        uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
        printf("%s\n",res_str);
        n = write(sock,res_str,strlen(res_str));
        if (n < 0) 
            error("ERROR writing to socket");
        return 101;
    }else if(friend_num == TOX_FAERR_ALREADYSENT){
        err = json_object_set(res,"type",json_string("RES"));
        mjson_error(err);
        err = json_object_set(res,"status",json_string("OK"));
        mjson_error(err);
        err = json_object_set(res,"retcode",json_integer(103));
        mjson_error(err);
        err = json_object_set(res,"description",json_string("you are already friend"));
        mjson_error(err);
        err = json_object_set(res,"mode",json_string("SERVER"));
        mjson_error(err);
        err = json_object_set(res,"cmd",json_string("ADD_FRIEND"));
        mjson_error(err);
        err = json_object_set(res,"from",json_string("LOCALHOST"));
        mjson_error(err);
        err = json_object_set(res,"remoteID",json_string(friendID));
        mjson_error(err);
        uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
        printf("%s\n",res_str);
        n = write(sock,res_str,strlen(res_str));
        if (n < 0) 
            error("ERROR writing to socket");
        return 103;
    }
    else{
        printf("ADD_FRIEND:Failed %d",friend_num);
        err = json_object_set(res,"type",json_string("RES"));
        mjson_error(err);
        err = json_object_set(res,"status",json_string("ERROR"));
        mjson_error(err);
        err = json_object_set(res,"retcode",json_integer(102));
        mjson_error(err);
        err = json_object_set(res,"description",json_string("add friend failed"));
        mjson_error(err);
        err = json_object_set(res,"mode",json_string("SERVER"));
        mjson_error(err);
        err = json_object_set(res,"cmd",json_string("ADD_FRIEND"));
        mjson_error(err);
        err = json_object_set(res,"from",json_string("LOCALHOST"));
        mjson_error(err);
        err = json_object_set(res,"remoteID",json_string(friendID));
        mjson_error(err);
        uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
        printf("%s\n",res_str);
        n = write(sock,res_str,strlen(res_str));
        if (n < 0) 
            error("ERROR writing to socket");
        return 102;
    }
}

int mtox_friend_accept(Tox *m, const char *friendID, int sock){
    printf("ADD_FRIEND:%s\n",friendID);
    int friend_num = tox_add_friend_norequest(m,friendID);
    json_t *res = json_object();
    int n;
    int err;
    if(friend_num == -1){
        //check if already added
        friend_num = tox_get_friend_number(m,friendID);
        if(friend_num != -1){
            err = json_object_set(res,"type",json_string("RES"));
            mjson_error(err);
            err = json_object_set(res,"status",json_string("OK"));
            mjson_error(err);
            err = json_object_set(res,"retcode",json_integer(103));
            mjson_error(err);
            err = json_object_set(res,"description",json_string("you are already friend"));
            mjson_error(err);
            err = json_object_set(res,"mode",json_string("SERVER"));
            mjson_error(err);
            err = json_object_set(res,"cmd",json_string("ADD_FRIEND"));
            mjson_error(err);
            err = json_object_set(res,"from",json_string("LOCALHOST"));
            mjson_error(err);
            err = json_object_set(res,"remoteID",json_string(friendID));
            mjson_error(err);
            uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
            printf("%s\n",res_str);
            n = write(sock,res_str,strlen(res_str));
            if (n < 0) 
                error("ERROR writing to socket");
            return 103;
        }
        
        printf("ADD_FRIEND:Failed");
        err = json_object_set(res,"type",json_string("RES"));
        mjson_error(err);
        err = json_object_set(res,"status",json_string("ERROR"));
        mjson_error(err);
        err = json_object_set(res,"retcode",json_integer(102));
        mjson_error(err);
        err = json_object_set(res,"description",json_string("add friend failed"));
        mjson_error(err);
        err = json_object_set(res,"mode",json_string("SERVER"));
        mjson_error(err);
        err = json_object_set(res,"cmd",json_string("ADD_FRIEND"));
        mjson_error(err);
        err = json_object_set(res,"from",json_string("LOCALHOST"));
        mjson_error(err);
        err = json_object_set(res,"remoteID",json_string(friendID));
        mjson_error(err);
        uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
        printf("%s\n",res_str);
        n = write(sock,res_str,strlen(res_str));
        if (n < 0) 
            error("ERROR writing to socket");
        return 102;
    }
    err = json_object_set(res,"type",json_string("RES"));
    mjson_error(err);
    err = json_object_set(res,"status",json_string("OK"));
    mjson_error(err);
    err = json_object_set(res,"retcode",json_integer(101));
    mjson_error(err);
    err = json_object_set(res,"description",json_string("add friend success"));
    mjson_error(err);
    err = json_object_set(res,"mode",json_string("SERVER"));
    mjson_error(err);
    err = json_object_set(res,"cmd",json_string("ADD_FRIEND"));
    mjson_error(err);
    err = json_object_set(res,"from",json_string("LOCALHOST"));
    mjson_error(err);
    err = json_object_set(res,"remoteID",json_string(friendID));
    mjson_error(err);
    uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
    printf("%s\n",res_str);
    n = write(sock,res_str,strlen(res_str));
    if (n < 0) 
         error("ERROR writing to socket");
    return 101;
}

/**
 * 最大的msg最大长度采用1024,如果过长就由node端程序进行分包，对方接收到之后再由node端进行合并，最后把完整的包
 * 传输到远端
 */

int mtox_send_message(Tox *m, const char *friendID, int sock,json_t *msg_json){
    const uint8_t *message = json_dumps(msg_json,JSON_INDENT(4));
    printf("SEND_MESSAGE:%s\n",friendID);
    int32_t friend_num = tox_get_friend_number(m,friendID);
    json_t *res = json_object();
    int n;
    int err;
    if(friend_num == -1){
        // add friend first
        err = json_object_set(res,"type",json_string("RES"));
        mjson_error(err);
        err = json_object_set(res,"status",json_string("ERROR"));
        mjson_error(err);
        err = json_object_set(res,"retcode",json_integer(202));
        mjson_error(err);
        err = json_object_set(res,"description",json_string("target is not a friend"));
        mjson_error(err);
        err = json_object_set(res,"mode",json_string("SERVER"));
        mjson_error(err);
        err = json_object_set(res,"cmd",json_string("SEND_MESSAGE"));
        mjson_error(err);
        err = json_object_set(res,"from",json_string("LOCALHOST"));
        mjson_error(err);
        err = json_object_set(res,"remoteID",json_string(friendID));
        mjson_error(err);
        uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
        printf("%s\n",res_str);
        n = write(sock,res_str,strlen(res_str));
        if (n < 0) 
            error("ERROR:writing socket\n");
        return 202;
    }
    printf("friendNum:%d\n",friend_num);
    uint32_t message_id = tox_send_message(m,friend_num,"hi",strlen("hi"));
    if(message_id == 0){
        // message send failed
        err = json_object_set(res,"type",json_string("RES"));
        mjson_error(err);
        err = json_object_set(res,"status",json_string("ERROR"));
        mjson_error(err);
        err = json_object_set(res,"retcode",json_integer(203));
        mjson_error(err);
        err = json_object_set(res,"description",json_string("send message failed"));
        mjson_error(err);
        err = json_object_set(res,"mode",json_string("SERVER"));
        mjson_error(err);
        err = json_object_set(res,"cmd",json_string("SEND_MESSAGE"));
        mjson_error(err);
        err = json_object_set(res,"from",json_string("LOCALHOST"));
        mjson_error(err);
        err = json_object_set(res,"remoteID",json_string(friendID));
        mjson_error(err);
        uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
        printf("%s\n",res_str);
        n = write(sock,res_str,strlen(res_str));
        if (n < 0) 
            error("ERROR:writing socket\n");
        return 203;
    }
    //message send success
    err = json_object_set(res,"type",json_string("RES"));
    mjson_error(err);
    err = json_object_set(res,"status",json_string("OK"));
    mjson_error(err);
    err = json_object_set(res,"retcode",json_integer(201));
    mjson_error(err);
    err = json_object_set(res,"description",json_string("send message success"));
    mjson_error(err);
    err = json_object_set(res,"mode",json_string("SERVER"));
    mjson_error(err);
    err = json_object_set(res,"cmd",json_string("SEND_MESSAGE"));
    mjson_error(err);
    err = json_object_set(res,"from",json_string("LOCALHOST"));
    mjson_error(err);
    err = json_object_set(res,"remoteID",json_string(friendID));
    mjson_error(err);
    uint8_t *res_str = json_dumps(res,JSON_INDENT(4));
    printf("%s\n",res_str);
    n = write(sock,res_str,strlen(res_str));
    if (n < 0) 
        error("ERROR:writing socket\n");
    return 201;
}