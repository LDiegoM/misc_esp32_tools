#ifndef settings_h
#define settings_h

#include <vector>
#include <ArduinoJson.h>
#include <internal/settings/models.hpp>
#include <internal/core/storage.h>
#include <internal/core/logging.h>
#include <internal/garage_door/garage_door.hpp>

extern const char* SETTINGS_FILE;

class Settings {
    private:
        Application *m_app;
        Storage *m_storage;
        settings_t m_settings;
        bool m_settingsOK;
        GarageDoor *m_garageDoor;

        bool readSettings();
        String createJson();
        void defaultSettings();

    public:
        Settings(Application *app, GarageDoor *garageDoor);

        bool begin();
        bool isSettingsOK();
        settings_t getSettings();
        bool saveSettings();

        void setDeviceValue(String deviceID);
        void setDeviceValue(String deviceID, String geoLocationLat, String geoLocationLng);

        void addWifiAP(const char* ssid, const char* password);
        bool updWifiAP(const char* ssid, const char* password);
        bool delWifiAP(const char* ssid);
        bool ssidExists(String ssid);

        void setMQTTValues(String server, String username, String password, uint16_t port, uint16_t sendPeriod);
        void setMQTTValues(String server, String username, uint16_t port, uint16_t sendPeriod);
        bool setMQTTCertificate(String certData);

        void setLoggingValues(uint8_t level, uint16_t refreshPeriod);

        void setDateValues(String server1, String server2, long gmtOffset, int daylightOffset);

        void setGarageDoorValues(uint8_t doorOpenWarningTime, uint16_t refreshDoorStatusTime);
};

#endif
