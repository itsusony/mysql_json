#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <string.h>
#include "libs/parson.h"

my_bool json_init(UDF_INIT *initid, UDF_ARGS *args, char *message){
    if(args->arg_count<2){
        strcpy(message, "json() requires two arguments");
        return 1;
    }
    else if(args->arg_type[0] != STRING_RESULT || args->arg_type[1] != STRING_RESULT){
        strcpy(message, "json() requires string type args");
        return 1;
    }
    else if(args->lengths[0] == 0 || args->lengths[1] == 0){
        strcpy(message, "json() requires args not null");
        return 1;
    }
    initid->ptr = calloc(10240,sizeof(char));
    return 0;
}

void json_deinit(UDF_INIT *initid){
    free(initid->ptr);
}

char *json(UDF_INIT *initid, UDF_ARGS *args,
      char *result, unsigned long *length,
      char *is_null, char *error)
{
    JSON_Value *jv = json_parse_string(args->args[0]);
    JSON_Value_Type json_type = json_value_get_type(jv);
    if(json_type==JSONError){
        *length=0;
        *is_null=1;
        *error=1;
        return NULL;
    }else{
        if(json_type == JSONObject){
            JSON_Object *o = json_value_get_object(jv);
            JSON_Value *value = json_object_dotget_value(o,args->args[1]);
            if(value==NULL){
                *length = 0;
                *is_null = 0;
                return NULL;
            }else{
                *is_null = 0;
                switch(json_value_get_type(value)){
                    case JSONArray:
                        *is_null = 1;
                        break;
                    case JSONError:
                    case JSONObject:
                        *is_null = 1;
                        *error = 1;
                        break;
                    case JSONString:
                        strcpy(initid->ptr,((char*)json_value_get_string(value)));
                        break;
                    case JSONNumber:
                        sprintf(initid->ptr,"%f",json_value_get_number(value));
                        break;
                    case JSONNull:
                        *is_null = 1;
                        break;
                    case JSONBoolean:
                        sprintf(initid->ptr,"%d",json_value_get_boolean(value));
                        break;
                }
                *length = initid->ptr == NULL ? 0 : strlen(initid->ptr); 
            }
            json_value_free(value);
        }else{
            *is_null = 1;
        }
        return initid->ptr; 
    }
}
