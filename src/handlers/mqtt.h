#ifndef handlers_mqtt_h
#define handlers_mqtt_h

#include <internal/core/mqtt_connection.h>

struct handlers_mqtt_t {
    mqtt_t connection;
    uint16_t sendPeriod;
};

#endif
