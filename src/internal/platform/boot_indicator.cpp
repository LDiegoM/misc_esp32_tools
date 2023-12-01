#include <internal/platform/boot_indicator.h>

//////////////////// Constructor
BootIndicator::BootIndicator(uint8_t pinBootIndicator, bool indicatorStartsOn) {
    m_pinBootIndicator = pinBootIndicator;
    m_indicatorIsOn = indicatorStartsOn;

    pinMode(m_pinBootIndicator, OUTPUT);
    digitalWrite(m_pinBootIndicator, indicatorLevel());
}

//////////////////// Public methods implementation
void BootIndicator::alternateOnOff() {
    m_indicatorIsOn = !m_indicatorIsOn;
    digitalWrite(m_pinBootIndicator, indicatorLevel());
}

void BootIndicator::turnOn() {
    m_indicatorIsOn = true;
    digitalWrite(m_pinBootIndicator, indicatorLevel());
}

void BootIndicator::turnOff(){
    m_indicatorIsOn = false;
    digitalWrite(m_pinBootIndicator, indicatorLevel());
}

void BootIndicator::setBlinkTime(unsigned long millis) {
    if (m_tmrBlink != nullptr) {
        m_tmrBlink->stop();
        free(m_tmrBlink);
    }

    m_tmrBlink = new Timer(millis);
    m_tmrBlink->start();
}

void BootIndicator::stopBlink() {
    if (m_tmrBlink == nullptr)
        return;

    m_tmrBlink->stop();
    free(m_tmrBlink);
    m_tmrBlink = nullptr;
}

void BootIndicator::setIndicatorStatusCallback(bool(*callback)()) {
    indicatorCallback = callback;
}

void BootIndicator::loop() {
    if (m_tmrBlink != nullptr) {
        if (m_tmrBlink->isTime())
            alternateOnOff();
        
        return;
    }

    // If timer is null, then try to execute a callback to indicate indicator status
    if (indicatorCallback != nullptr) {
        m_indicatorIsOn = indicatorCallback();
        digitalWrite(m_pinBootIndicator, m_indicatorIsOn);
    }
}

//////////////////// Private methods implementation
int BootIndicator::indicatorLevel() {
    return (m_indicatorIsOn == true ? HIGH : LOW);
}
