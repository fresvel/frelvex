#include <esp_http_server.h>
esp_err_t ws_app_send_resp(httpd_req_t *req);
esp_err_t ws_process_binary(httpd_ws_frame_t *ws_pkt,httpd_req_t *req);
esp_err_t ws_process_close(httpd_ws_frame_t *ws_pkt,httpd_req_t *req);
esp_err_t ws_process_ping(httpd_ws_frame_t *ws_pkt,httpd_req_t *req);
esp_err_t ws_process_pong(httpd_req_t *req);
esp_err_t ws_process_text(httpd_ws_frame_t *ws_pkt,httpd_req_t *req);
