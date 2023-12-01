#ifndef handlers_http_h
#define handlers_http_h

#include <WebServer.h>
#include <ArduinoJson.h>

#include <handlers/models.h>
#include <internal/platform/storage.h>
#include <internal/platform/wifi_connection.h>
#include <internal/settings/settings.h>
#include <internal/platform/date_time.h>
#include <internal/platform/logging.h>

/////////// HTTP Handlers
void downloadLogs();
void deleteLogs();
void restart();
void getSettings();
void delSettings();

void getBootstrapCSS();
void getBootstrapJS();
void getNotFound();

void getStatus();

void getSettingsWiFi();
void addSettingsWiFi();
void updSettingsWiFi();
void delSettingsWiFi();

void getSettingsMQTT();
void updSettingsMQTT();
void getSettingsMQTTCert();

void getSettingsDate();
void updSettingsDate();

void getSettingsLogging();
void updSettingsLogging();

void getAdmin();

class HttpHandlers {
    private:
        const uint16_t HTTP_PORT = 80;

        const char* BOOTSTRAP_CSS = "https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css";
        const char* BOOTSTRAP_JS = "https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js";

        const char* CONNECTED = "Connected";
        const char* DISCONNECTED = "Disconnected";

        const char* MSG_OK = "ok";
        const char* ERR_SETTINGS_SAVE_GENERIC = "Error saving settings. Please try again";
        const char* ERR_WIFI_AP_NOT_FOUND = "AP ssid not found";
        const char* ERR_WIFI_AP_IS_EMPTY = "AP ssid can't be empty";
        const char* ERR_WIFI_AP_EXISTS = "There's already an AP with the same SSID";

        const char* ERR_MQTT_IS_EMPTY = "MQTT parameters can't be empty";
        
        const char* ERR_DATE_IS_EMPTY = "Date Time parameters can't be empty";
        
        const char* ERR_LOGGING_IS_EMPTY = "Logging parameters can't be empty";

        WiFiConnection *m_wifi;
        Storage *m_storage;
        Settings *m_settings;
        WebServer *m_server;
        DateTime *m_dateTime;

        void defineRoutes();

        String getHeaderHTML(String section);
        String getFooterHTML(String page, String section);
        
        String getStatusHTML();

        String getSettingsWiFiHTML();
        String getSettingsMQTTHTML();
        String getSettingsDateHTML();
        String getSettingsLoggingHTML();

        String getAdminHTML();

        request_wifiAP_t parseWiFiBody(String body);
        std::vector<wifiAP_t> parseMultiWiFiBody(String body);

        request_mqtt_t parseMQTTBody(String body);

        request_dateTime_t parseDateBody(String body);

        request_logging_t parseLoggingBody(String body);

    public:
        HttpHandlers(WiFiConnection *wifi, Storage *storage, Settings *settings, DateTime *dateTime);

        bool begin();
        void loop();

        // HTTP handlers
        void handleDownloadLogs();
        bool handleDeleteLogs();
        void handleRestart();
        void handleGetSettings();
        void handleDelSettings();

        void handleGetBootstrapCSS();
        void handleGetBootstrapJS();
        void handleGetNotFound();

        void handleGetStatus();

        void handleGetSettingsWiFi();
        void handleAddSettingsWiFi();
        void handleUpdSettingsWiFi();
        void handleDelSettingsWiFi();

        void handleGetSettingsMQTT();
        void handleUpdSettingsMQTT();
        void handleGetSettingsMQTTCert();

        void handleGetSettingsDate();
        void handleUpdSettingsDate();

        void handleGetSettingsLogging();
        void handleUpdSettingsLogging();

        void handleGetAdmin();
};

extern HttpHandlers *httpHandlers;

#endif
