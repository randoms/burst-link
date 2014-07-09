#include <string.h>
#include <jansson.h>

int main(int argc, char *argvs[]){
    char *text = "{\"name\":\"yeah\"}";
    json_t *root;
    json_error_t error;
    root = json_loads(text,0,&error);
    if(!root){
        fprintf(stderr,"error:on line %d,%s",error.line,error.text);
        return 1;
    }
    printf("%s\n",json_string_value(json_object_get(root,"name")));
}