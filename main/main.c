#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <gpio_main.h>
#include <uart_main.h>
#include <wifi_main.h>
#include <fsys_main.h>
#include <ws_text.h>
#include <ota_main.h>

void funcion_send(void*param, char*buffer){
    if (buffer!=NULL)
    {
        //printf("Función send %s\n", buffer);    
    }else{ 
        printf("Función send finalizada\n");
    }
    
}

void app_main(void)
{


//char *json_str = "{\"type\":\"tipo de ws\",\"data\":\"datos de proceso\"}";
//ws_app_types(json_str);


config_gpio();
config_uart();
config_wifi();

config_fsys();
read_file("/spiffs/html/panel.html", funcion_send,NULL);

//funcion_ota();
printf("\n*****Iniciando OTA*****\n");
config_ota();
printf("\n*****Fin de OTA*****");
//ota_main();
}
