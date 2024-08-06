#include <internal/sensors/sensors.h>

//////////////////// Constructor
Sensors::Sensors(uint8_t dhtPin, uint8_t dhtType) {
    m_dhtPin = dhtPin;
    m_dhtType = dhtType;
    m_updated = false;

    m_tempValue = 0;
    m_humiValue = 0;

    m_dhtSensor = new DHT(m_dhtPin, m_dhtType);

    m_tmrRefreshValues = new Timer(REFRESH_TIME_MILLIS);
}

//////////////////// Public methods implementation
bool Sensors::begin() {
    Timer *timeOut = new Timer(10 * 1000);
    timeOut->start();

    m_dhtSensor->begin();

    delay(1000);

    // Do a first value read
    readValues();

    startCycle();

    return true;
}

void Sensors::startCycle() {
    m_tmrRefreshValues->start();
}

void Sensors::stopCycle() {
    m_tmrRefreshValues->stop();
}

void Sensors::loop() {
    if (!m_tmrRefreshValues->isRunning()) {
        return;
    }

    if (m_tmrRefreshValues->isTime()) {
        readValues();
    }
}

bool Sensors::updated() {
    return m_updated;
}

float Sensors::temp() {
    m_updated = false;
    return m_tempValue;
}

float Sensors::humi() {
    m_updated = false;
    return m_humiValue;
}

//////////////////// Private methods implementation
void Sensors::readValues() {
    m_tempValue = m_dhtSensor->readTemperature();
    if (isnan(m_tempValue))
        m_tempValue = 0;

    m_humiValue = m_dhtSensor->readHumidity();
    if (isnan(m_humiValue))
        m_humiValue = 0;

    m_updated = true;
}
