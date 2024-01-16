#include <handlers/mqtt.h>

MqttHandlers *mqttHandlers = nullptr;

//////////////////// Constructor
MqttHandlers::MqttHandlers(Doorbell *doorbell) {
    m_doorbell = doorbell;
}

//////////////////// Public methods implementation
void MqttHandlers::begin() {
    if (_mqtt == nullptr)
        return;

    _mqtt->subscribe(MQTT_TOPIC_DOOR_BELL);

    lg->debug("mqtt_handlers.begin() - registering handler callback", __FILE__, __LINE__);
    _mqtt->setCallback([](char* topic, uint8_t* payload, unsigned int length){
        mqttHandlers->processReceivedMessage(topic, payload, length);
    });
}

void MqttHandlers::processReceivedMessage(char* topic, uint8_t* payload, unsigned int length) {
    if (_mqtt == nullptr)
        return;

    String sTopic = String(topic);
    lg->debug("mqtt_handlers.processReceivedMessage() - message was received", __FILE__, __LINE__, lg->newTags()->add("topic", sTopic));

    _mqtt->processReceivedMessage(topic, payload, length);

    if (!sTopic.equals(MQTT_TOPIC_DOOR_BELL)) {
        return;
    }

    lg->debug("Message received from doorbell topic. Composing incoming message.", __FILE__, __LINE__);
    String incomingMessage = "";
    for (unsigned int i = 0; i < length; i++)
        incomingMessage += (char)payload[i];
    
    lg->debug("incomingMessage from topic", __FILE__, __LINE__,
        lg->newTags()->add("message", incomingMessage)
    );

    if (incomingMessage.equals("RING")) {
        ringDoorbell();
        return;
    }
}

void MqttHandlers::loop() {
    if (_mqtt == nullptr)
        return;
    
    _mqtt->loop();
}

//////////////////// Private methods implementation
void MqttHandlers::ringDoorbell() {
    if (m_doorbell->isPlaying())
        return;

    if (m_doorbell->play())
        lg->info("doorbell is ringing", __FILE__, __LINE__);
    else
        lg->error("fail to ring doorbell", __FILE__, __LINE__);
}
