#ifndef boot_indicator_h
#define boot_indicator_h

#include <internal/platform/timer.h>

class BootIndicator {
    private:
        uint8_t m_pinBootIndicator;
        bool m_indicatorIsOn = false;
        Timer *m_tmrBlink = nullptr;
        bool(*indicatorCallback)() = nullptr;

        int indicatorLevel();

    public:
        BootIndicator(uint8_t pinBootIndicator, bool indicatorStartsOn = false);

        void alternateOnOff();
        void turnOn();
        void turnOff();
        void setBlinkTime(unsigned long milis);
        void stopBlink();
        void setIndicatorStatusCallback(bool(*callback)());
        void loop();
};

#endif
