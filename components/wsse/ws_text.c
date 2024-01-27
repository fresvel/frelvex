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
    char *ws_fn;
}ws_send_file_t;



static void ws_send_files_init(void *param, char *buffer, u_int8_t state){

        ESP_LOGI(TAG,"Sending files for websocket");
        ws_send_file_t *ws_sf=(ws_send_file_t *)param;
        httpd_req_t *req=(httpd_req_t*)ws_sf->req;
        char* ws_fn=(char*)ws_sf->ws_fn;

        printf("\033[0;35m");
        printf("Valor de ws_fn: %s\n",ws_fn);



        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "function", (char*)ws_sf->ws_fn);
        cJSON_AddStringToObject(root, "data", buffer);
        cJSON_AddNumberToObject(root, "state",state);

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


static void ws_json_config_path(cJSON **json_files_path){
*json_files_path=cJSON_CreateObject();
cJSON_AddItemToObject(*json_files_path,"ws-body",cJSON_CreateString("/files/html/body"));
cJSON_AddItemToObject(*json_files_path,"ws-header",cJSON_CreateString("/files/html/header"));
cJSON_AddItemToObject(*json_files_path,"ws-footer",cJSON_CreateString("/files/html/footer")); 
cJSON_AddItemToObject(*json_files_path,"ws-css",cJSON_CreateString("/files/css/lib"));
cJSON_AddItemToObject(*json_files_path,"ws-js",cJSON_CreateString("/files/js/lib"));
cJSON_AddItemToObject(*json_files_path,"ws-system",cJSON_CreateString("/files/system"));
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

    if (cJSON_IsArray(ws_info))
    {
        ESP_LOGI(TAG, "The ws-info element is an Array, requesting files\n");
        int arr_size=cJSON_GetArraySize(ws_info);
        for (size_t i = 0; i < arr_size; i++)
        {
            cJSON *item = cJSON_GetArrayItem(ws_info,i);
            if (cJSON_IsString(item))
            {
                ws_send_file_t ws_file;
                ws_file.req=req;
                ws_file.ws_fn=ws_strtype;
                char* base_path=cJSON_GetObjectItem(json_files_path,ws_strtype)->valuestring;
                char ws_path[strlen(ws_strtype)+strlen(base_path)+1];
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

    }else if (cJSON_IsObject(ws_info))
    {
        ESP_LOGI(TAG, "The ws-info element is an Objetc\n");
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
