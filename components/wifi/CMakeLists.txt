idf_component_register(SRCS "wifi_main.c"
                    INCLUDE_DIRS "include"
                    REQUIRES nvs_flash esp_wifi wsse)
