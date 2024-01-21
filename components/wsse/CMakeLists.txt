idf_component_register(SRCS "ws_text.c" "ws_main.c" "ws_app.c" "ws_get.c" "ws_post.c" "keep_alive.c"
                    INCLUDE_DIRS "include"
                    EMBED_TXTFILES "certs/servercert.pem" "certs/prvtkey.pem"
                    REQUIRES esp_https_server wifi esp_timer fsys app_update json)
