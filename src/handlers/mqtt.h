#ifndef handlers_mqtt_h
#define handlers_mqtt_h

#include <internal/core/mqtt_connection.h>
#include <internal/audio/doorbell.h>

struct handlers_mqtt_t {
    mqtt_t connection;
    uint16_t sendPeriod;
};

class MqttHandlers {
    private:
        const char* MQTT_TOPIC_DOOR_BELL = "topic-door-bell";

        Doorbell *m_doorbell;

        void ringDoorbell();

    public:
        MqttHandlers(Doorbell *doorbell);

        void begin();
        void processReceivedMessage(char* topic, uint8_t* payload, unsigned int length);
        void loop();
};

extern MqttHandlers *mqttHandlers;

#endif
