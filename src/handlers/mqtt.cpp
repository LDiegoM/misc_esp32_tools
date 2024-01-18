#include <handlers/mqtt.h>

MqttHandlers *mqttHandlers = nullptr;

//////////////////// Constructor
MqttHandlers::MqttHandlers(GarageDoor *garageDoor) {
    m_garageDoor = garageDoor;
}

//////////////////// Public methods implementation
void MqttHandlers::begin() {
    if (_mqtt == nullptr)
        return;

    _mqtt->subscribe(MQTT_TOPIC_GARAGE_DOOR);

    lg->debug("mqtt_handlers.begin() - registering handler callback", __FILE__, __LINE__);
    _mqtt->setCallback([](char* topic, uint8_t* payload, unsigned int length){
        mqttHandlers->processReceivedMessage(topic, payload, length);
    });
}

void MqttHandlers::processReceivedMessage(char* topic, uint8_t* payload, unsigned int length) {
    if (_mqtt == nullptr)
        return;

    String sTopic = String(topic);

    _mqtt->processReceivedMessage(topic, payload, length);

    if (!sTopic.equals(MQTT_TOPIC_GARAGE_DOOR)) {
        return;
    }

    lg->debug("Message received from garage door topic. Composing incoming message.", __FILE__, __LINE__);
    String incomingMessage = "";
    for (unsigned int i = 0; i < length; i++)
        incomingMessage += (char)payload[i];
    
    lg->debug("incomingMessage from topic", __FILE__, __LINE__,
        lg->newTags()->add("message", incomingMessage)
    );

    if (incomingMessage.equals("RING")) {
        m_garageDoor->ringDoorbell();
    } else if (incomingMessage.equals("OPEN")) {
        m_garageDoor->openDoor();
    } else if (incomingMessage.equals("CLOSE")) {
        m_garageDoor->closeDoor();
    }
}

void MqttHandlers::loop() {
    if (_mqtt == nullptr)
        return;
    
    _mqtt->loop();
}

//////////////////// Private methods implementation
