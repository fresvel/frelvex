#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "fsys_main.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"
#include "string.h"


typedef void (*file_send_t)(void *req, char* buffer, uint8_t state, int part);

static const char* TAG="File System";
void fsys_init(void){
ESP_LOGI(TAG, "config_fsys\n");

esp_vfs_spiffs_conf_t fs_cfg={
    .base_path= "/files",
    .partition_label=NULL,
    .max_files=10,
    .format_if_mount_failed=true
};

ESP_ERROR_CHECK(esp_vfs_spiffs_register(&fs_cfg));
ESP_LOGI(TAG, "Sistema de Archivos montado de forma correcta\n");
ESP_ERROR_CHECK(esp_spiffs_check(fs_cfg.partition_label));
ESP_LOGI(TAG, "Partición del sistema de archivos sin errores\n");
size_t total = 0, used=0;
ESP_ERROR_CHECK(esp_spiffs_info(fs_cfg.partition_label,&total,&used));
ESP_LOGI(TAG, "Información de la partición de archivos %d usado de %d\n", used, total);
ESP_LOGI(TAG, "Abriendo archivo");
}

esp_err_t fsys_xFuntion_file(char *path, file_send_t file_send, void *param){
    FILE* file=fopen(path, "r");
if (file==NULL)
{
    ESP_LOGE(TAG, "Error abriendo archivo : %s\n", path);
    return ESP_FAIL;
}

 size_t stack_free = uxTaskGetStackHighWaterMark(NULL);
 printf("Tamaño libre del stack: %u bytes\n", stack_free);


//size_t heap_size=esp_get_free_heap_size();
printf("\033[0;35m");
//printf("Espacio de memoria ram disponible %d bytes\n", heap_size);

fseek(file,0,SEEK_END);
long file_size=ftell(file)+1;
ESP_LOGI(TAG, "Tamaño del archivo: %ld bytes\n", file_size);
size_t block_size=(size_t)stack_free; //25% de la memoria ram disponible debido a que 4 byes son 32 bits
printf("\033[0;36m");
printf("Tamaño del bloque de lectura: %d bytes\n", block_size);
fseek(file,0,SEEK_SET);
printf("Antes del if\n");
if(block_size<file_size){
    printf("Reservando variable\n");
    char buffer[block_size+1];
    printf("Variable reservada\n");
    int end=(int)(file_size/block_size);
    for (int i = 0; i <=end ; i++)
    {
    end=(int)(file_size/block_size);
    uint8_t state=(uint8_t)((100*i/end));
    printf("State*** %d *** end%d\n",state,end);
    //printf("Leyendo bloque\n");
    memset(buffer,0,block_size+1);
    fread(buffer,1,block_size,file);
    //printf("%s",buffer); 
    file_send(param, buffer,state,i);

    if (i%40==0)
    {
        ESP_LOGI(TAG, "Time for refresh proccesses\n");
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    
    

    }
}else{
    char buffer[file_size];
    memset(buffer,0,file_size);
    fread(buffer,1,file_size,file);
    //printf("%s",buffer);
    file_send(param, buffer,(uint8_t)100, (int)0);

}
    //file_send(param, NULL);//Indica que finaliza el envío del file
    printf("\033[0;31m");
    printf("The file was printed successfully");

fclose(file);
return ESP_OK;
}