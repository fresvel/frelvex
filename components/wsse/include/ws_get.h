#include <esp_https_server.h>
#include <esp_http_server.h>
 extern const httpd_uri_t ota_uri;
 extern const httpd_uri_t get_panel; //Si se usa  es una declaración global
 extern const httpd_uri_t ota_post_uri;
 extern const httpd_uri_t ws_apphtml_uri;
 extern const httpd_uri_t ws_logoimg_uri;
 extern const httpd_uri_t ws_bulmacss_uri;
 extern const httpd_uri_t chart_js_uri;
 extern const httpd_uri_t ws_appjs_uri;
 extern const httpd_uri_t panel_js_uri;                                   //sin  cada archivo que incluye la librería
                                    //crea su propia copia