#ifndef display_h
#define display_h

#include <TFT_ILI9163C.h>

#include <internal/core/date_time.h>
#include <internal/display/number_set.h>
#include <internal/sensors/sensors.h>
#include <internal/core/timer.h>
#include <internal/core/wifi_connection.h>

// Color definitions
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GRAY 0x8410
#define DARK_GRAY 0x4A69

class Display {
    private:
        TFT_ILI9163C* m_tft;
        uint16_t m_backgroundColor;
        uint16_t m_foreColor;
        Sensors* m_sensors;
        WiFiConnection* m_wifi;
        DateTime* m_dateTime;

        NumberSet *m_tempRep, *m_ddmmRep, *m_yearRep, *m_timeRep;
        NumberSet *m_humiRep;
        bool m_apMode;
        Timer *m_tmrRefreshTime;

        void drawScreen();
        void printDateTime();
        void printValues();

    public:
        Display(TFT_ILI9163C *tft, uint16_t backgroundColor, uint16_t foreColor, Sensors *sensors,
                WiFiConnection *wifi, DateTime *dateTime);

        bool begin();
        void loop();

};

#endif
