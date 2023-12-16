#include <internal/core/mqtt_connection.h>

//////////////////// Constructor
MqttConnection::MqttConnection(mqtt_t settings, WiFiConnection *wifi, Storage *storage) {
    m_settings = settings;
    m_wifi = wifi;
    m_storage = storage;
}

//////////////////// Public methods implementation

//////////////////// Private methods implementation
