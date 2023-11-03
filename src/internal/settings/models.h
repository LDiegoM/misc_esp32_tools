#ifndef settings_models_h
#define settings_models_h

#include <Arduino.h>
#include <vector>
#include <internal/platform/mqtt.h>
#include <internal/platform/wifi_connection.h>
#include <internal/platform/date_time.h>

struct settings_t {
    mqtt_t mqtt;
    std::vector<wifiAP_t> wifiAPs;
    struct logger {
        String outputPath;
        uint16_t writePeriod;
    } logger;
    dateTime_t dateTime;
};

#endif
