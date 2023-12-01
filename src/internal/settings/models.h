#ifndef settings_models_h
#define settings_models_h

#include <Arduino.h>
#include <vector>
#include <internal/platform/mqtt.h>
#include <internal/platform/wifi_connection.h>
#include <internal/platform/date_time.h>
#include <internal/platform/logging.h>

struct settings_t {
    mqtt_t mqtt;
    std::vector<wifiAP_t> wifiAPs;
    dateTime_t dateTime;
    logging_t logging;
};

#endif
