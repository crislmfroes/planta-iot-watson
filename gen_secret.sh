#!/bin/bash
if [ $# -eq 7 ] 
then
    printf "#define SECRET_SSID '$1'\n#define SECRET_PASS '$2'\n#define SECRET_CH_ID $3\n#define SECRET_WRITE_APIKEY '$4'\n#define SUBSCRIBE_TOPIC '$5'\n#define PUBLISH_TOPIC '$6'\n#define ID_MQTT '$7'" > src/secrets.h
fi