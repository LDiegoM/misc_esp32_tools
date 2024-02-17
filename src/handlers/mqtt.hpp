#ifndef handlers_mqtt_h
#define handlers_mqtt_h

#include <internal/core/mqtt_connection.h>
#include <internal/garage_door/garage_door.hpp>

struct handlers_mqtt_t {
    mqtt_t connection;
    uint16_t sendPeriod;
};

class MqttHandlers {
    private:
        const char* MQTT_TOPIC_GARAGE_DOOR = "topic-garage-door";

        GarageDoor *m_garageDoor;

    public:
        MqttHandlers(GarageDoor *garageDoor);

        void begin();
        void processReceivedMessage(char* topic, uint8_t* payload, unsigned int length);
        void loop();
};

extern MqttHandlers *mqttHandlers;

#endif
