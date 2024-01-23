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





static const char *TAG="ws-text";
typedef struct {
    char *type;
    char *info;
} ws_appjson_t;

typedef struct {
    char *type;
    char *info;
} ws_onload_t;


// Recibe un json de {"tag":"", "data":{}}
// Devuelve el type y un string de los datos
static esp_err_t ws_getjson_tags(const char *json_string, ws_appjson_t *ws_object, const char *tag, const char *info) { 
    printf("Iniciando JSON\n");

    cJSON *root = cJSON_Parse(json_string);
    printf("conversión realizada\n");
    if (root == NULL) {
        // Manejar el error de análisis JSON
        printf("Error parsing JSON\n");
        return ESP_FAIL;
    }

    printf("Datos obtenidos de json\n");

    cJSON *type = cJSON_GetObjectItem(root, tag);
    cJSON *data = cJSON_GetObjectItem(root, info);
    
    if (type == NULL||data == NULL) 
    {
        ESP_LOGE(TAG, "Etiquetas no encontradas");
        return ESP_FAIL;
    }
    
    char* data_str=cJSON_PrintUnformatted(data);

    printf("Datos obtenidos de json %s\n",data_str);

    if (type != NULL && data != NULL) {

        //printf("Dentro del IF\n");
        // Evitar errores si el objeto no tiene las claves esperadas
        size_t len_type = strlen(type->valuestring);
        printf("Longitud de type ok %d\n",len_type);
        size_t len_data = strlen(data_str);
        printf("Longitud de data lendata %d\n",len_data);
        // Asignar memoria para ws_object
        ws_object->type = malloc(strlen(type->valuestring) + 1); // +1 para el carácter nulo
        printf("Buffer type ok\n");
        ws_object->info = malloc(strlen(data_str) + 1);
        printf("Buffer data ok\n");
        // Copiar datos
        strcpy(ws_object->type, type->valuestring);
        printf("copia type ok\n");
        strcpy(ws_object->info, data_str);
        printf("copia data ok\n");
        printf("GET JSON TYPE: %s\n", ws_object->type);
        printf("GET JSON DATA: %s\n", ws_object->info);
    }

    // Liberar memoria
    cJSON_Delete(root);
    return ESP_OK;
}

static esp_err_t ws_getarr_files(const char *json_str, void *ws_arr_name,cJSON **json_files_path){
    ESP_LOGI(TAG, "Array processing for %s type", (char*)ws_arr_name);
    cJSON *root=cJSON_Parse(json_str);
    


    cJSON *path = cJSON_GetObjectItem(*json_files_path, ws_arr_name);
    char *jsonString = cJSON_Print(*json_files_path);
    printf("\n\n\n**********%s\n\n\n\n", jsonString);
    //Add item control for null data
    printf("\n\n\n**********%s\n\n\n", path->valuestring);

    if (root == NULL)
    {
        ESP_LOGE(TAG, "Error parsing JSON");
        return ESP_FAIL;
    }
    
    cJSON *arr_json=cJSON_GetObjectItem(root,ws_arr_name);
    
    if (!cJSON_IsArray(arr_json))
    {
        cJSON_Delete(root);
        ESP_LOGW(TAG,"Error parsing");
        return ESP_FAIL;
    }

    int arr_size=cJSON_GetArraySize(arr_json);
    char *arr_str[arr_size];
    ESP_LOGW(TAG,"Array size %d",arr_size);
    for (size_t i = 0; i < arr_size; i++)
    {
        printf("processing%d\n",i);
        cJSON *item=cJSON_GetArrayItem(arr_json, i);
        printf("ITEM: %s\n",item->valuestring);
        if (cJSON_IsString(item))
        {
            arr_str[i] = strdup(item->valuestring);
            printf("ITEM$$$: %s\n",arr_str[i]);

            
        }else{
            ESP_LOGE(TAG,"The element is not a string in the position: %d\n", i);
            printf("COntrol");
            cJSON_Delete(root);
            return ESP_FAIL;
        }
        
    }

    printf("Outside for\n");
    for (size_t i = 0; i < arr_size; i++)
    {
        printf("new for %d\n",i);
        printf("ITEM+++: %s\n",arr_str[i]);
        ESP_LOGI(TAG,"item: %s\n", arr_str[i]);
    }

    ESP_LOGE(TAG,"TEST: %s\n",arr_str[0]);

    for (size_t i = 0; i < arr_size; i++)
    {
        free(arr_str[i]);
    }
    ESP_LOGI(TAG,"HERE IT IS NECESSARY TO PROCESS THE ARRAY");
    cJSON_Delete(root);
    return ESP_OK;
    


}

char *ws_setjson_app(const ws_appjson_t *ws_object) {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "type", ws_object->type);
    cJSON_AddStringToObject(root, "data", ws_object->info);

    // Imprimir el JSON creado
    char *ws_json_str = cJSON_PrintUnformatted(root);
    printf("Created JSON: %s\n", ws_json_str);

    cJSON_Delete(root);
    return ws_json_str;
}





static void ws_app_body(char * data_str){}

static void ws_app_data(char * data_str){}

static void ws_app_header(char * data_str){}

static void ws_app_section(char * data_str){}

static void ws_app_footer(char * data_str){}

static esp_err_t ws_app_system(char * data_str, httpd_req_t *req, cJSON **json_files_path){
    ESP_LOGI(TAG, "WS_APP_SYSTEM");
    ws_appjson_t ws_sys_obj;
    esp_err_t ret= ws_getjson_tags(data_str, &ws_sys_obj, "ws-method","ws-request");
    if (ret==ESP_OK)
    {
        if (strcmp(ws_sys_obj.type, "ws-onload")==0) //Here it is managed the ws-onload structure 
        {
            ESP_LOGI(TAG, "WS_APP_SYSTEM type ws-onload");
            ws_getarr_files(ws_sys_obj.info, "ws-header",json_files_path);   //Get the header
            ws_getarr_files(ws_sys_obj.info, "ws-body", json_files_path);     //Get
            ws_getarr_files(ws_sys_obj.info, "ws-lib-js", json_files_path);   //Get the javascript libraries requested 
            ws_getarr_files(ws_sys_obj.info, "ws-lib-js", json_files_path);   //Get the css libraries requested
        }

        printf("JSON to Object: Method: %s Data: %s\n", ws_sys_obj.type, ws_sys_obj.info);
        free(ws_sys_obj.type);
        free(ws_sys_obj.info);
    }
    return ESP_OK;

}

static void ws_app_default(char * data_str){}


static void ws_json_config_path(cJSON **json_files_path){
*json_files_path=cJSON_CreateObject();
cJSON_AddItemToObject(*json_files_path,"ws-body",cJSON_CreateString("/files/html/body"));
cJSON_AddItemToObject(*json_files_path,"ws-header",cJSON_CreateString("/files/html/header"));
cJSON_AddItemToObject(*json_files_path,"ws-footer",cJSON_CreateString("/files/html/footer")); 
cJSON_AddItemToObject(*json_files_path,"ws-lib-css",cJSON_CreateString("/files/css/lib"));
cJSON_AddItemToObject(*json_files_path,"ws-lib-js",cJSON_CreateString("/files/js/lib"));
cJSON_AddItemToObject(*json_files_path,"ws-app-js",cJSON_CreateString("/files/js/app"));
cJSON_AddItemToObject(*json_files_path,"ws-app-css",cJSON_CreateString("/files/css/app"));
cJSON_AddItemToObject(*json_files_path,"ws-system",cJSON_CreateString("/files/system"));
}

void ws_app_text(char *ws_app_str, httpd_req_t *req) {


    /*THIS WILL BE TO SEND TO MAIN OR TO A CONFIG OR INIT FUNCTION*/
    cJSON *json_files_path=NULL;
    ws_json_config_path(&json_files_path);

    char *jsonString = cJSON_Print(json_files_path);
    printf("%s\n", jsonString);
    
    ESP_LOGI(TAG, "Datos recibidos %s", ws_app_str);
    // Convertir JSON a objeto
    ws_appjson_t ws_app_obj;
    esp_err_t ret= ws_getjson_tags(ws_app_str, &ws_app_obj,"ws-type","ws-info");

    
    if (ret == ESP_OK)
    {
        printf("JSON to Object: Type: %s Data: %s\n", ws_app_obj.type, ws_app_obj.info);
        if (strcmp(ws_app_obj.type, "ws-body")==0){
            ws_app_body(ws_app_obj.info);
            //función para devolver
        }else if (strcmp(ws_app_obj.type, "ws-section")==0){
            ws_app_section(ws_app_obj.info);
        }else if (strcmp(ws_app_obj.type, "ws-data")==0){
            ws_app_data(ws_app_obj.info);
        }else if (strcmp(ws_app_obj.type, "ws-header")==0){
            ws_app_header(ws_app_obj.info);
        }else if (strcmp(ws_app_obj.type, "ws-footer")==0){
            ws_app_footer(ws_app_obj.info);
        }else if (strcmp(ws_app_obj.type, "ws-system")==0){
            ws_app_system(ws_app_obj.info, req,&json_files_path);
        }else{
            ws_app_default(ws_app_obj.info);
        }
        free(ws_app_obj.type);
        free(ws_app_obj.info);        
    }
    
    

    /*
    // Convertir objeto a JSON
    ws_appjson_t obj = {"tipo de ws", "datos de proceso"};
    printf("Iniciando conversión\n");

    char *json_str = ws_setjson_app(&obj);
    printf("Object to JSON:\n%s\n", json_str);
    
    // Liberar memoria
    free(json_str);*/

    
    

}
