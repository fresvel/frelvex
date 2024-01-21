#include <ws_app.h>
#include <esp_https_server.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include "esp_ota_ops.h"
#include <fsys.h>
#include <keep_alive.h>

static char *TAG="ws-app";

static esp_ota_handle_t update_handle = 0 ; //Mandar a un puntero para manejar desde distintas funciones

static void httpd_send_file(void*param,char * buffer){ //Duplicated in ws-get.c the idea not to use ws_get
    httpd_req_t *req =(httpd_req_t*)param;
    httpd_resp_sendstr_chunk(req,buffer);
    printf("Sending file...\n");
}



static void ws_ota_init(){

        esp_err_t err;        
        const esp_partition_t *update_partition = NULL;

        ESP_LOGI(TAG, "Starting OTA web server task");

        const esp_partition_t *configured = esp_ota_get_boot_partition();
        const esp_partition_t *running = esp_ota_get_running_partition();

        if (configured != running) {
            ESP_LOGW(TAG, "Configured OTA boot partition at offset 0x%08"PRIx32", but running from offset 0x%08"PRIx32,
                    configured->address, running->address);
            ESP_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
        }
        ESP_LOGI(TAG, "Running partition: %s", running->label);

        update_partition = esp_ota_get_next_update_partition(NULL);
        assert(update_partition != NULL);
        ESP_LOGI(TAG, "Writing to partition: %s",update_partition->label);

        
        err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
            esp_ota_abort(update_handle);
            //task_fatal_error();
        }
        ESP_LOGI(TAG, "esp_ota_begin succeeded");
    }

static void ws_ota_finish(){
        esp_err_t err = esp_ota_end(update_handle);
        if (err != ESP_OK) {
            if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
                ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            } else {
                ESP_LOGE(TAG, "esp_ota_end failed (%s)!", esp_err_to_name(err));
            }
            //task_fatal_error();
            ESP_LOGE(TAG,"OTA END FAILED!!\n");

        }

        err = esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL));
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)!", esp_err_to_name(err));
            //task_fatal_error();
        }
        ESP_LOGI(TAG, "Prepare to restart system!");
        
}



esp_err_t ws_process_text(httpd_ws_frame_t *ws_pkt,httpd_req_t *req){ 
    ESP_LOGI(TAG, "Received packet with message: %s", ws_pkt->payload);
    esp_err_t ret;
    if(ws_pkt->payload[0]=='a')
    ws_ota_init();
    if(ws_pkt->payload[0]=='b'){
        ws_ota_finish();
        char*data="100";
        httpd_ws_frame_t send_pkt;
        memset(&send_pkt, 0, sizeof(httpd_ws_frame_t));
        send_pkt.payload = (uint8_t*)data;
        send_pkt.len = strlen(data);
        send_pkt.type = HTTPD_WS_TYPE_TEXT;
        ret = httpd_ws_send_frame(req, &send_pkt);//////***
        vTaskDelay(1000/portTICK_PERIOD_MS);
        
        esp_restart();
    }
     ret = httpd_ws_send_frame(req, ws_pkt);//////***
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }
    ESP_LOGI(TAG, "ws_handler: httpd_handle_t=%p, sockfd=%d, client_info:%d", req->handle,
                httpd_req_to_sockfd(req), httpd_ws_get_fd_info(req->handle, httpd_req_to_sockfd(req)));
    return ret;
    }

esp_err_t ws_process_pong(httpd_req_t *req){ 
    ESP_LOGD(TAG, "Received PONG message");
    return wss_keep_alive_client_is_active(httpd_get_global_user_ctx(req->handle),
            httpd_req_to_sockfd(req));
    }

esp_err_t ws_process_ping(httpd_ws_frame_t *ws_pkt,httpd_req_t *req){ 
    // Response PONG packet to peer
    ESP_LOGI(TAG, "Got a WS PING frame, Replying PONG");
    ws_pkt->type = HTTPD_WS_TYPE_PONG;
    esp_err_t ret = httpd_ws_send_frame(req, ws_pkt);//////***
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }
    ESP_LOGI(TAG, "ws_handler: httpd_handle_t=%p, sockfd=%d, client_info:%d", req->handle,
                httpd_req_to_sockfd(req), httpd_ws_get_fd_info(req->handle, httpd_req_to_sockfd(req)));
    return ret;
    }
esp_err_t ws_process_close(httpd_ws_frame_t *ws_pkt,httpd_req_t *req){
// Response CLOSE packet with no payload to peer
    ws_pkt->len = 0;
    ws_pkt->payload = NULL;
    esp_err_t ret = httpd_ws_send_frame(req, ws_pkt);//////***
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
    }
    ESP_LOGI(TAG, "ws_handler: httpd_handle_t=%p, sockfd=%d, client_info:%d", req->handle,
                httpd_req_to_sockfd(req), httpd_ws_get_fd_info(req->handle, httpd_req_to_sockfd(req)));
    return ret;
    }
esp_err_t ws_process_binary(httpd_ws_frame_t *ws_pkt,httpd_req_t *req){ 
        ESP_LOGI(TAG, "Processing binary request");
        char*data="77";
        httpd_ws_frame_t send_pkt;
        memset(&send_pkt, 0, sizeof(httpd_ws_frame_t));

        esp_err_t err = esp_ota_write( update_handle, (const void *)ws_pkt->payload,ws_pkt->len);
        if (err != ESP_OK) {
            esp_ota_abort(update_handle);
            ESP_LOGE(TAG,"OTA WRITE FAILED!!\n");
            data="101";
            //task_fatal_error();
        }

        send_pkt.payload = (uint8_t*)data;
        send_pkt.len = strlen(data);
        send_pkt.type = HTTPD_WS_TYPE_TEXT;


        err = httpd_ws_send_frame(req, &send_pkt);//////***
        //printf("%s",ws_pkt.payload);
        //printf("len: %d",ws_pkt.len);
    return ESP_OK;
}
