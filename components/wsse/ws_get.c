#include <ws_get.h>
#include <esp_https_server.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <fsys.h>

static const char * TAG="ws_get";


static void httpd_send_file(void*param,char * buffer){
    httpd_req_t *req =(httpd_req_t*)param;
    httpd_resp_sendstr_chunk(req,buffer);
    printf("Sending file...\n");
}



static esp_err_t paneljs_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected");
        
        httpd_resp_set_type(req, "text/javascript");
        read_file("/spiffs/js/panel.js",httpd_send_file,req);
        //httpd_resp_send(req, buffer,HTTPD_RESP_USE_STRLEN);
        
    }
    return ESP_OK;
    
}

 const httpd_uri_t panel_js_uri={
    .uri = "/panel.js",
    .method = HTTP_GET,
    .handler = paneljs_handler,
    .user_ctx = NULL
};


static esp_err_t ws_appthml_handler(httpd_req_t *req){
        ESP_LOGI(TAG, "New client connected, OTA HTML");
        httpd_resp_set_type(req, "text/html");
        read_file("/files/html/header/head.html",httpd_send_file,req); // envía los archivos
        //read_file("/files/html/header/header.html",httpd_send_file,req);
        read_file("/files/html/body/ota/ota.html",httpd_send_file,req);
        read_file("/files/html/footer/footer.html",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);//Finaliza el envío de los archivos
    return ESP_OK;
}

const httpd_uri_t ws_apphtml_uri={
    .uri = "/app.html",
    .method = HTTP_GET,
    .handler = ws_appthml_handler,
    .user_ctx = NULL
};

static esp_err_t ws_appjs_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected appjs");
        
        httpd_resp_set_type(req, "text/javascript");
        read_file("/files/js/footer/ws_app.js",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);
        
    }
    return ESP_OK;
    
}


 const httpd_uri_t ws_appjs_uri={
    .uri = "/ws_app.js",
    .method = HTTP_GET,
    .handler = ws_appjs_handler,
    .user_ctx = NULL
};


static esp_err_t chartjs_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected");
        httpd_resp_set_type(req, "text/javascript");
        read_file("/spiffs/js/chart.js",httpd_send_file,req);
        
    }
    return ESP_OK;      
}

 const httpd_uri_t chart_js_uri={
    .uri = "/chart.js",
    .method = HTTP_GET,
    .handler = chartjs_handler,
    .user_ctx = NULL
};


static esp_err_t ws_bulmacss_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected");
        
        httpd_resp_set_type(req, "text/css");
        read_file("/files/css/lib/bulma.css",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);
        
    }
    return ESP_OK;
    
}

 const httpd_uri_t ws_bulmacss_uri={
    .uri = "/bulma.css",
    .method = HTTP_GET,
    .handler = ws_bulmacss_handler,
    .user_ctx = NULL
};

static esp_err_t ws_logoimg_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected");
        httpd_resp_set_type(req, "image/svg+xml");
        read_file("/files/img/logo.svg",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);
    }
    return ESP_OK;      
}

 const httpd_uri_t ws_logoimg_uri={
    .uri = "/logo.svg",
    .method = HTTP_GET,
    .handler = ws_logoimg_handler,
    .user_ctx = NULL
};


static esp_err_t ota_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected, OTA HTML");
        httpd_resp_set_type(req, "text/html");
        read_file("/files/html/body/ota/ota.html",httpd_send_file,req);
        
    }
    return ESP_OK;
}

 const httpd_uri_t ota_uri={
    .uri = "/ota.html",
    .method = HTTP_GET,
    .handler = ota_handler,
    .user_ctx = NULL
};

 const httpd_uri_t ota_post_uri={
    .uri = "/ota_post",
    .method = HTTP_POST,
    .handler = ota_handler,
    .user_ctx = NULL
};

static esp_err_t panel_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected");
        
        httpd_resp_set_type(req, "text/html");
        read_file("/spiffs/html/panel.html",httpd_send_file,req);
        //httpd_resp_send(req, buffer,HTTPD_RESP_USE_STRLEN);
        
    }
    return ESP_OK;
    
}

const httpd_uri_t get_panel={
    .uri = "/panel",
    .method = HTTP_GET,
    .handler = panel_handler,
    .user_ctx = NULL
};