#include <ws_get.h>
#include <esp_https_server.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <fsys_main.h>

static const char * TAG="ws_get";


static void httpd_send_file(void*param, char *buffer, uint8_t state, int part){
    httpd_req_t *req =(httpd_req_t*)param;
    httpd_resp_sendstr_chunk(req,buffer);
    printf("Sending file...\n");
}


static esp_err_t index_html_handler(httpd_req_t *req){
        ESP_LOGI(TAG, "New client connected, OTA HTML");
        httpd_resp_set_type(req, "text/html");
        fsys_xFuntion_file("/files/index/index.html",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);//Finaliza el envío de los archivos
    return ESP_OK;
}

const httpd_uri_t index_html_uri={
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_html_handler,
    .user_ctx = NULL
};

static esp_err_t index_js_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected appjs");
        
        httpd_resp_set_type(req, "text/javascript");
        fsys_xFuntion_file("/files/index/index.js",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);
        
    }
    return ESP_OK;
    
}


 const httpd_uri_t index_js_uri={
    .uri = "/index.js",
    .method = HTTP_GET,
    .handler = index_js_handler,
    .user_ctx = NULL
};

static esp_err_t index_svg_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected");
        httpd_resp_set_type(req, "image/svg+xml");
        fsys_xFuntion_file("/files/index/index.svg",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);
    }
    return ESP_OK;      
}

 const httpd_uri_t index_svg_uri={
    .uri = "/index.svg",
    .method = HTTP_GET,
    .handler = index_svg_handler,
    .user_ctx = NULL
};


static esp_err_t index_css_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected, OTA HTML");
        httpd_resp_set_type(req, "text/css");
        fsys_xFuntion_file("/files/index/index.css",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);
    }
    return ESP_OK;
}

 const httpd_uri_t index_css_uri={
    .uri = "/index.css",
    .method = HTTP_GET,
    .handler = index_css_handler,
    .user_ctx = NULL
};


static esp_err_t main_css_handler(httpd_req_t *req){
    if (req->method == HTTP_GET){
        ESP_LOGI(TAG, "New client connected, OTA HTML");
        httpd_resp_set_type(req, "text/css");
        fsys_xFuntion_file("/files/lib/css/main.css",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);
    }
    return ESP_OK;
}

 const httpd_uri_t main_css_uri={
    .uri = "/main.css",
    .method = HTTP_GET,
    .handler = main_css_handler,
    .user_ctx = NULL
};

static esp_err_t main_js_handler(httpd_req_t *req){
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "New client connected appjs");
        
        httpd_resp_set_type(req, "text/javascript");
        fsys_xFuntion_file("/files/lib/js/main.js",httpd_send_file,req);
        httpd_resp_sendstr_chunk(req,NULL);
        
    }
    return ESP_OK;
    
}

 const httpd_uri_t main_js_uri={
    .uri = "/main.js",
    .method = HTTP_GET,
    .handler = main_js_handler,
    .user_ctx = NULL
};