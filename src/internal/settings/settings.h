#ifndef settings_h
#define settings_h

#include <vector>
#include <ArduinoJson.h>
#include <internal/settings/models.h>
#include <internal/platform/storage.h>
#include <internal/platform/logging.h>

extern const char* SETTINGS_FILE;

class Settings {
    private:
        Storage *m_storage;
        settings_t m_settings;
        bool m_settingsOK;

        bool readSettings();
        String createJson();
        void defaultSettings();

    public:
        Settings(Storage *storage);

        bool begin();
        bool isSettingsOK();
        settings_t getSettings();
        bool saveSettings();

        void addWifiAP(const char* ssid, const char* password);
        bool updWifiAP(const char* ssid, const char* password);
        bool delWifiAP(const char* ssid);
        bool ssidExists(String ssid);

        void setMQTTValues(String server, String username, String password, uint16_t port, uint16_t sendPeriod);
        void setMQTTValues(String server, String username, uint16_t port, uint16_t sendPeriod);
        bool setMQTTCertificate(String certData);

        void setLoggingValues(uint8_t level, uint16_t refreshPeriod);

        void setDateValues(String server1, String server2, long gmtOffset, int daylightOffset);
};

#endif
