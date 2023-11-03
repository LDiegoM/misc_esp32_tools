#ifndef handlers_models_h
#define handlers_models_h

#include <Arduino.h>

struct request_wifiAP_t {
    String ssid, password;
};

struct request_mqtt_t {
    String server, username, password, caCertPath;
    uint16_t port, sendPeriod;
    String certData;
};

struct request_dateTime_t {
    String server1;
    String server2;
    long gmtOffset;
    int daylightOffset;
};

#endif
