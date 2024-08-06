#ifndef sensors_h
#define sensors_h

#include <DHT.h>
#include <internal/core/timer.h>

class Sensors {
    private:
        uint8_t m_dhtPin, m_dhtType;
        bool m_updated;

        float m_tempValue, m_humiValue;
        DHT *m_dhtSensor;
        Timer *m_tmrRefreshValues;

        const uint16_t REFRESH_TIME_MILLIS = 500;

        void readValues();

    public:
        Sensors(uint8_t dhtPin, uint8_t dhtType);

        bool begin();
        void startCycle();
        void stopCycle();
        void loop();
        bool updated();
        float temp();
        float humi();
};

#endif
