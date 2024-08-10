#include <handlers/mqtt.h>

MqttHandlers *mqttHandlers = nullptr;

//////////////////// Constructor
MqttHandlers::MqttHandlers(GarageDoor *garageDoor, Sensors *sensors, uint16_t sendPeriod) {
    m_garageDoor = garageDoor;
    m_sensors = sensors;
    m_sendPeriod = sendPeriod;

    if (sendPeriod > 0) {
        m_tmrSend = new Timer(sendPeriod * 1000);
    }
}

//////////////////// Public methods implementation
void MqttHandlers::begin() {
    if (_mqtt == nullptr)
        return;

    _mqtt->subscribe(MQTT_TOPIC_GARAGE_DOOR);
    _mqtt->subscribe(MQTT_TOPIC_VEST_CMD);

    lg->debug("mqtt_handlers.begin() - registering handler callback", __FILE__, __LINE__);
    _mqtt->setCallback([](char* topic, uint8_t* payload, unsigned int length){
        mqttHandlers->processReceivedMessage(topic, payload, length);
    });

    if (m_tmrSend != nullptr) {
        m_tmrSend->start();
    }
}

void MqttHandlers::processReceivedMessage(char* topic, uint8_t* payload, unsigned int length) {
    if (_mqtt == nullptr)
        return;

    String sTopic = String(topic);

    _mqtt->processReceivedMessage(topic, payload, length);

    if (!sTopic.equals(MQTT_TOPIC_GARAGE_DOOR) && !sTopic.equals(MQTT_TOPIC_VEST_CMD)) {
        return;
    }

    lg->debug("Message received from expected topic. Composing incoming message.", __FILE__, __LINE__,
        lg->newTags()->add("topic", sTopic));
    String incomingMessage = "";
    for (unsigned int i = 0; i < length; i++)
        incomingMessage += (char)payload[i];
    
    lg->debug("incomingMessage from topic", __FILE__, __LINE__,
        lg->newTags()->add("message", incomingMessage)
    );

    if (sTopic.equals(MQTT_TOPIC_GARAGE_DOOR)) {
        if (incomingMessage.equals("RING")) {
            m_garageDoor->ringDoorbell();
        } else if (incomingMessage.equals("OPEN")) {
            m_garageDoor->openDoor();
        } else if (incomingMessage.equals("CLOSE")) {
            m_garageDoor->closeDoor();
        }
    } else if (sTopic.equals(MQTT_TOPIC_VEST_CMD)) {
        if (incomingMessage.equals("GET_HUMI")) {
            sendHumidity();
        } else if (incomingMessage.equals("GET_TEMP")) {
            sendTemperature();
        }
    }
}

void MqttHandlers::loop() {
    if (_mqtt == nullptr)
        return;
    
    _mqtt->loop();

    if (m_tmrSend != nullptr && m_tmrSend->isTime()) {
        sendHumidity();
    }
}

//////////////////// Private methods implementation
void MqttHandlers::sendHumidity() {
    _mqtt->publish(MQTT_TOPIC_VEST_HUMIDITY, String(m_sensors->humi()).c_str(), true);
}

void MqttHandlers::sendTemperature() {
    _mqtt->publish(MQTT_TOPIC_VEST_TEMPERATURE, String(m_sensors->temp()).c_str(), true);
}
