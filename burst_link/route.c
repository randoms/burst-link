#include <jansson.h>
#include "handler.h"
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



void route(char *cmds){
    json_t *cmd_json;
    json_error_t *error;
    const char *type, *mode, *cmd, *from, *remoteID;
    printf("route called");
    cmd_json = json_loads(cmds, 0, error);
    if(!cmd_json){
        // data is not a vaild json string
        printf("Not JSON");
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
        
        printf("JSON:type %s",type);
    }
}