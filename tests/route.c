#include "route.h"
/**
 * 请求的格式
var cmd = {
    'type':'REQ',
    'mode':'SERVER',
    'cmd':'ADD_FRIEND',
    'from':'LOCALHOST',
    'remoteID':remote_tox_id,
}*/

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



void route(const char * cmds, Tox *m, int sock){
    json_t *cmd_json;
    json_error_t error;
    const char *type;
    const char *mode;
    const char *cmd;
    const char *from;
    const char *remoteID;
    const char *STATIC_TYPE_REQ = "REQ";
    const char *STATIC_TYPE_RES = "RES";
    const char *STATIC_MODE_SERVER = "SERVER";
    const char *STATIC_MODE_CLIENT = "CLIENT";
    const char *STATIC_CMD_ADD_FRIEND = "ADD_FRIEND";
    const char *STATIC_CMD_SEND_MESSAGE = "SEND_MESSAGE";
    const char *STATIC_FROM_LOCALHOST = "LOCALHOST";
    const char *STATIC_FROM_REMOTE = "REMOTE";
    

    cmd_json = json_loads(cmds, 0, &error);
    if(!cmd_json){
        // data is not a vaild json string
        printf("Not JSON\n");
    }else{
        type = json_string_value(
            json_object_get(cmd_json,"type"));
        mode = json_string_value(
            json_object_get(cmd_json,"mode"));
        cmd = json_string_value(
            json_object_get(cmd_json,"cmd"));
        from = json_string_value(
            json_object_get(cmd_json,"from"));
        remoteID = json_string_value(
            json_object_get(cmd_json,"remoteID"));
        
        if(strcmp(type,STATIC_TYPE_REQ) == 0 && strcmp(from,STATIC_FROM_LOCALHOST) == 0){
            // req from localhost
            if(strcmp(cmd,STATIC_CMD_ADD_FRIEND) == 0)
                mtox_friend_add(m,remoteID,sock);
            else if(strcmp(cmd,STATIC_CMD_SEND_MESSAGE) == 0){
                json_t *msg = json_object_get(cmd_json,"msg");
                mtox_send_message(m,remoteID,sock,msg);
            }
        }
    }
    json_decref(cmd_json);
}