#ifndef mqtt_platform_h
#define mqtt_platform_h

#include <Arduino.h>

struct mqtt_t {
    String server, username, password, caCertPath;
    uint16_t port, sendPeriod;
    char *ca_cert;
};

#endif
