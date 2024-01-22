#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <ws_text.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>
#include <cJSON.h>



static const char *TAG="ws-text";
typedef struct {
    char *type;
    char *info;
} ws_appjson_t;

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

static void ws_app_system(char * data_str){
    ws_appjson_t ws_sys_obj;
    esp_err_t ret= ws_getjson_tags(data_str, &ws_sys_obj, "ws-method","ws-request");
    
    
    
    if (ret==ESP_OK)
    {
        printf("JSON to Object: Method: %s Data: %s\n", ws_sys_obj.type, ws_sys_obj.info);
        free(ws_sys_obj.type);
        free(ws_sys_obj.info);
    }
    

}

static void ws_app_default(char * data_str){}

void ws_app_text(char *ws_app_str) {
    
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
            ws_app_system(ws_app_obj.info);
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
