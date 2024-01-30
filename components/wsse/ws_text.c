#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <esp_http_server.h>
#include <ws_text.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>
#include <cJSON.h>
#include <fsys_main.h>

#include "esp_heap_caps.h"//DEPURACIÓN



static const char *TAG="ws-text";
typedef struct {
    char *type;
    char *info;
} ws_strjson_t;

typedef struct {
    char *type;
    char *info;
} ws_onload_t;

typedef struct {
    httpd_req_t *req;
    char *method;
    char *fname;
    char *module;
    char *object;
}ws_send_file_t;



static void ws_send_files_init(void *param, char *buffer, u_int8_t state, int tally){

        ESP_LOGI(TAG,"Sending files for websocket");
        ws_send_file_t *ws_file=(ws_send_file_t *)param;
        httpd_req_t *req=(httpd_req_t*)ws_file->req;
        char* method=(char*)ws_file->method;
        char* fname=(char*)ws_file->fname;
        char* module=(char*)ws_file->module;
        char* object=(char*)ws_file->object;

        printf("\033[0;35m");
        printf("Valor de method: %s\n",method);


        cJSON *load=cJSON_CreateObject();
        cJSON_AddNumberToObject(load, "state",state);
        cJSON_AddNumberToObject(load, "tally", tally);

        cJSON *src=cJSON_CreateObject();
        cJSON_AddStringToObject(src, "module",module);
        cJSON_AddStringToObject(src, "file",fname); //validate if really it is necessary, maybe it is only needed object
        cJSON_AddStringToObject(src, "object",object);


        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "method",method); //module es ws-type
        cJSON_AddItemToObject(root, "load",load);
        cJSON_AddItemToObject(root, "src",src);
        
        cJSON_AddStringToObject(root, "data", buffer);


    // Imprimir el JSON creado
    char *ws_json_str = cJSON_PrintUnformatted(root);
    printf("Created JSON: %s\n", ws_json_str);

    

        httpd_ws_frame_t send_pkt;
        memset(&send_pkt, 0, sizeof(httpd_ws_frame_t));
        send_pkt.payload = (uint8_t*)ws_json_str;
        send_pkt.len = strlen(ws_json_str);
        send_pkt.type = HTTPD_WS_TYPE_TEXT;
        esp_err_t ret = httpd_ws_send_frame(req, &send_pkt);
        int mem=heap_caps_get_free_size(MALLOC_CAP_8BIT);
        printf("Memoria libre: %d\n", mem);
    free(ws_json_str);
    cJSON_Delete(root);
}


char *ws_setjson_app(const ws_strjson_t *ws_object) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", ws_object->type);
    cJSON_AddStringToObject(root, "data", ws_object->info);

    // Imprimir el JSON creado
    char *ws_json_str = cJSON_PrintUnformatted(root);
    printf("Created JSON: %s\n", ws_json_str);

    cJSON_Delete(root);
    return ws_json_str;
}





static void ws_app_data(char * data_str){

}

static esp_err_t ws_app_system(char * data_str, httpd_req_t *req, cJSON **json_files_path){
    ESP_LOGI(TAG, "WS_APP_SYSTEM");
    return ESP_OK;

}

static void ws_app_default(char * data_str){}


static void ws_json_config_path(cJSON **json_files_path){ //Desde system se puede crear una función para crear nuevos directorios virtuales
*json_files_path=cJSON_CreateObject();
cJSON_AddItemToObject(*json_files_path,"ws-body",cJSON_CreateString("/files/html/body"));
cJSON_AddItemToObject(*json_files_path,"ws-header",cJSON_CreateString("/files/html/header"));
cJSON_AddItemToObject(*json_files_path,"ws-footer",cJSON_CreateString("/files/html/footer")); 
cJSON_AddItemToObject(*json_files_path,"ws-css",cJSON_CreateString("/files/css"));
cJSON_AddItemToObject(*json_files_path,"ws-js",cJSON_CreateString("/files/js"));
cJSON_AddItemToObject(*json_files_path,"ws-system",cJSON_CreateString("/files/system"));
cJSON_AddItemToObject(*json_files_path,"ws-section",cJSON_CreateString("/files/section"));
}

void ws_app_text(char *ws_app_str, httpd_req_t *req) {

    /*THIS WILL BE TO SEND TO MAIN OR TO A CONFIG OR INIT FUNCTION*/
    cJSON *json_files_path=NULL;
    ws_json_config_path(&json_files_path);
    char *jsonString = cJSON_Print(json_files_path);
    printf("%s\n", jsonString);
    ESP_LOGE(TAG, "Datos recibidos %s", ws_app_str);
    // Convertir JSON a objeto
    ws_strjson_t ws_app_obj;
    cJSON *ws_obj_req=cJSON_Parse(ws_app_str);

    if(ws_obj_req==NULL){
        ESP_LOGE(TAG, "Error! Not posible to get object from string: %s", ws_app_str);
        return;
    }
    
    cJSON *ws_type=cJSON_GetObjectItem(ws_obj_req,"ws-type");
    if (!cJSON_IsString(ws_type))
    {
        ESP_LOGI(TAG, "The element ws-type must be a string\n");
        return;
    }

    char* ws_strtype=ws_type->valuestring;
    printf("ws-type %s\n", ws_type->valuestring);
    
    cJSON *ws_info=cJSON_GetObjectItem(ws_obj_req,"ws-info");

    ws_send_file_t ws_file;
    ws_file.req=req;
    ws_file.module=ws_strtype;
    ws_file.object="";

    char* base_path=cJSON_GetObjectItem(json_files_path,ws_strtype)->valuestring;
/*ARRAY REQUEST BASE*/
    if (cJSON_IsArray(ws_info))
    {
        ESP_LOGI(TAG, "The ws-info element is an Array, requesting files\n");
        int arr_size=cJSON_GetArraySize(ws_info);
        ws_file.method="render";
        for (size_t i = 0; i < arr_size; i++)
        {
            cJSON *item = cJSON_GetArrayItem(ws_info,i);
            if (cJSON_IsString(item))
            {
                ws_file.fname=item->valuestring;
                char ws_path[strlen(item->valuestring)+strlen(base_path)+1]; //POSIBLE REPORT
                sprintf(ws_path,"%s/%s",base_path,item->valuestring);
                printf("Path value to get file %s\n",ws_path); 
                fsys_xFuntion_file(ws_path,ws_send_files_init,&ws_file);
                //free(str_path);
                //Una vez que finaliza envíar señal de fin      
            }else{
                ESP_LOGE(TAG,"The element is not a string in the position: %d\n", i);
                printf("COntrol");
                cJSON_Delete(item);
                return ;
            }

        }
/*OBJETC REQUEST BASE*/
    }else if (cJSON_IsObject(ws_info))
    {
        ESP_LOGI(TAG, "The ws-info element is an Objetc\n");



        if (strcmp("ws-header",ws_type->valuestring) == 0)
        {
            ESP_LOGI(TAG, "Setting method for: %s\n",ws_type->valuestring);
            ws_file.method="header";
        }else{
            ESP_LOGI(TAG, "Setting alternative method for: %s\n",ws_type->valuestring);
            ws_file.method="render";
        }
            

        
      
        
        int arr_size=cJSON_GetArraySize(ws_info);
        printf("Array size: %d\n",arr_size);
        for (size_t i = 0; i < arr_size; i++)
        {
            cJSON *item = cJSON_GetArrayItem(ws_info,i);
            if (cJSON_IsString(item))
            {
                ws_file.fname=item->valuestring;
                ws_file.object=item->string;
                char ws_path[strlen(item->valuestring)+strlen(base_path)+1]; //POSIBLE REPORT
                sprintf(ws_path,"%s/%s",base_path,item->valuestring);
                printf("Path value to get file %s\n",ws_path); 
                fsys_xFuntion_file(ws_path,ws_send_files_init,&ws_file);
                //free(str_path);
                //Una vez que finaliza envíar señal de fin      
            }else{
                ESP_LOGE(TAG,"The element is not a string in the position: %d\n", i);
                printf("COntrol");
                cJSON_Delete(item);
                return ;
            }
        }
        


    

/*STRING REQUEST BASE*/    
    }else if (cJSON_IsString(ws_info)){
        ESP_LOGI(TAG, "The ws-info element is an String\n");
        
        ws_file.fname=ws_info->valuestring;
        ws_file.method="render";
        char ws_path[strlen(ws_info->valuestring)+strlen(base_path)+1];
        sprintf(ws_path,"%s/%s",base_path,ws_info->valuestring);
        printf("Path value to get file %s\n",ws_path); 
        fsys_xFuntion_file(ws_path,ws_send_files_init,&ws_file);
    
/*NOT IMPLEMENTED*/    
    }else{
        ESP_LOGI(TAG, "The ws-info element must be an array or object type");
        return;
    }



/*tttttttttttttttttttt*/
    
    ESP_LOGE(TAG, "End of new method");
    
    /*
    // Convertir objeto a JSON
    ws_strjson_t obj = {"tipo de ws", "datos de proceso"};
    printf("Iniciando conversión\n");

    char *json_str = ws_setjson_app(&obj);
    printf("Object to JSON:\n%s\n", json_str);
    
    // Liberar memoria
    free(json_str);*/

    
    

}
