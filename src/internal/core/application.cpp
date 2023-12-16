#include <internal/core/application.h>

//////////////////// Constructor
Application::Application(String name) {
    m_app.name = name;
    m_app.deviceID = getDeviceID();
    lg = new Logging(LOG_LEVEL_WARNING);
}
Application::Application(String name, uint8_t logLevel) {
    m_app.name = name;
    m_app.deviceID = getDeviceID();
    lg = new Logging(logLevel);
}
Application::Application(String name, uint16_t pinBootIndicator) {
    m_app.name = name;
    m_app.deviceID = getDeviceID();
    lg = new Logging(LOG_LEVEL_WARNING);
    m_bootIndicator = new BootIndicator(pinBootIndicator);
}
Application::Application(String name, uint16_t pinBootIndicator, uint8_t logLevel) {
    m_app.name = name;
    m_app.deviceID = getDeviceID();
    lg = new Logging(logLevel);
#ifdef ESP8266
    m_bootIndicator = new BootIndicator(pinBootIndicator, false, true);
#else
    m_bootIndicator = new BootIndicator(pinBootIndicator);
#endif
}
Application::~Application() {
    if (m_storage != nullptr) {
        free(m_storage);
        m_storage = nullptr;
    }

    if (m_bootIndicator != nullptr) {
        free(m_bootIndicator);
        m_bootIndicator = nullptr;
    }

    if (m_wifi != nullptr) {
        free(m_wifi);
        m_wifi = nullptr;
    }

    if (m_dateTime != nullptr) {
        free(m_dateTime);
        m_dateTime = nullptr;
    }
}

//////////////////// Public methods implementation
String Application::name() {
    return m_app.name;
}

void Application::setDeviceID(String deviceID) {
    m_app.deviceID = deviceID;
}
String Application::deviceID() {
    return m_app.deviceID;
}
void Application::setGeoLocation(geoLocation_t geoLocation) {
    m_app.geoLocation.s = geoLocation.s;
    m_app.geoLocation.w = geoLocation.w;
}
void Application::setGeoLocation(float geoLocationS, float geoLocationW) {
    m_app.geoLocation.s = geoLocationS;
    m_app.geoLocation.w = geoLocationW;
}
geoLocation_t Application::geoLocation() {
    return m_app.geoLocation;
}

bool Application::beginStorage() {
    if (m_storage == nullptr)
        m_storage = new Storage();
    
    while (!m_storage->begin()) {
        Serial.print(".");
    }
    lg->setStorage(m_storage);
    return true;
}
Storage* Application::storage() {
    return m_storage;
}

BootIndicator* Application::bootIndicator() {
    return m_bootIndicator;
}

void Application::beginWiFi(std::vector<wifiAP_t> wifiAPs) {
    if (m_wifi != nullptr)
        free(m_wifi);
    m_wifi = new WiFiConnection(wifiAPs);
    m_wifi->begin();
}
void Application::beginWiFi(std::vector<wifiAP_t> wifiAPs, String apSSID) {
    if (m_wifi != nullptr)
        free(m_wifi);
    m_wifi = new WiFiConnection(wifiAPs, apSSID);
    m_wifi->begin();
}
void Application::beginWiFi(String apSSID) {
    if (m_wifi != nullptr)
        free(m_wifi);
    m_wifi = new WiFiConnection(apSSID);
    m_wifi->begin();
}
WiFiConnection* Application::wifi() {
    return m_wifi;
}

bool Application::beginDateTime(dateTime_t settings) {
    if (m_dateTime != nullptr)
        free(m_dateTime);
    m_dateTime = new DateTime(settings);
    bool ok = m_dateTime->begin();
    lg->setDateTime(m_dateTime);
    return ok;
}
DateTime* Application::dateTime() {
    return m_dateTime;
}

void Application::loop() {
    if (m_bootIndicator != nullptr)
        m_bootIndicator->loop();

    if (lg != nullptr)
        lg->loop();
}

//////////////////// Private methods implementation
String Application::getDeviceID() {
    // TODO: Implement a default proper device id
    return name() + String("_device");
}
