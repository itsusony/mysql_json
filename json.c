#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <string.h>
#include "parson.h"

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
    return 0;
}

void json_deinit(UDF_INIT *initid){
}

char *json(UDF_INIT *initid, UDF_ARGS *args,
      char *result, unsigned long *length,
      char *is_null, char *error)
{
    JSON_Value *jv = json_parse_string(args->args[0]);
    JSON_Value_Type json_type = json_value_get_type(jv);
    if(json_type==JSONError){
        result=NULL;
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
                JSON_Array *arr;
                switch(json_value_get_type(value)){
                    case JSONArray:
                        arr = json_value_get_array(value);
                        int i;for (i = 0; i < json_array_get_count(arr); i++) {
                            JSON_Value *arr_v = json_array_get_value(arr,i);
                            if(json_value_get_type(arr_v)==JSONString){
                                strcat(result,json_value_get_string(arr_v));
                                strcat(result,"\n");
                            }else{
                                int len = strlen(result);
                                if(len>0)memset(result,0,len);
                                *is_null = 1;
                                break;
                            }
                        }
                        break;
                    case JSONError:
                    case JSONObject:
                        *is_null = 1;
                        *error = 1;
                        break;
                    case JSONString:
                        result = (char*)json_value_get_string(value);
                        break;
                    case JSONNumber:
                        sprintf(result,"%f",json_value_get_number(value));
                        break;
                    case JSONNull:
                        *is_null = 1;
                        break;
                    case JSONBoolean:
                        sprintf(result,"%d",json_value_get_boolean(value));
                        break;
                }
                *length = result == NULL ? 0 : strlen(result); 
            }
        }else{
            *is_null = 1;
        }
        return result; 
    }

}


