#ifndef handlers_mqtt_h
#define handlers_mqtt_h

#include <internal/core/mqtt_connection.h>
#include <internal/core/timer.h>
#include <internal/garage_door/garage_door.h>
#include <internal/sensors/sensors.h>

struct handlers_mqtt_t {
    mqtt_t connection;
    uint16_t sendPeriod;
};

class MqttHandlers {
    private:
        const char* MQTT_TOPIC_GARAGE_DOOR = "topic-garage-door";
        const char* MQTT_TOPIC_VEST_CMD = "topic-vest-cmd";
        const char* MQTT_TOPIC_VEST_HUMIDITY = "topic-vest-humi";
        const char* MQTT_TOPIC_VEST_TEMPERATURE = "topic-vest-temp";

        GarageDoor *m_garageDoor;
        Sensors *m_sensors;
        uint16_t m_sendPeriod;
        Timer *m_tmrSend = nullptr;

    public:
        MqttHandlers(GarageDoor *garageDoor, Sensors *sensors, uint16_t sendPeriod);

        void begin();
        void processReceivedMessage(char* topic, uint8_t* payload, unsigned int length);
        void loop();
    
    private:
        void sendHumidity();
        void sendTemperature();
};

extern MqttHandlers *mqttHandlers;

#endif
