#ifndef mqtt_connection_h
#define mqtt_connection_h

#include <Arduino.h>
#include <PubSubClient.h>
#include <internal/core/wifi_connection.h>
#include <internal/core/logging.h>
#include <internal/core/storage.h>

struct mqtt_t {
    String server, username, password, caCertPath;
    uint16_t port, sendPeriod;
    char *ca_cert;
};

class MqttConnection {
    private:
        mqtt_t m_settings;
        WiFiConnection *m_wifi;
        Storage *m_storage;

    public:
        MqttConnection(mqtt_t settings, WiFiConnection *wifi, Storage *storage);
};

#endif
