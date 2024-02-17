#ifndef handlers_http_h
#define handlers_http_h

#ifdef ESP8266
#include <ESP8266WebServer.h>
#elif defined(ESP32)
#include <WebServer.h>
#else
#error "Unsupported platform"
#endif

#include <vector>
#include <ArduinoJson.h>

#include <handlers/models.h>
#include <internal/core/storage.h>
#include <internal/core/wifi_connection.h>
#include <internal/settings/settings.hpp>
#include <internal/core/date_time.h>
#include <internal/core/logging.h>
#include <internal/garage_door/garage_door.hpp>
#include <internal/statistics/statistics.hpp>

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

        const char* ERR_EMPTY_PARAMETERS = "parameters can't be empty";
        
        Application *m_app;
        Settings *m_settings;
        GarageDoor *m_garageDoor;
        Statistics *m_statistics;
#ifdef ESP8266
        ESP8266WebServer *m_server;
#elif defined(ESP32)
        WebServer *m_server;
#endif

        void defineRoutes();

        String getHeaderHTML(String section);
        String getFooterHTML();
        
        String getNotFoundHTML();
        String getStatusGlobalHTML();
        String getStatusStatisticsHTML();

        String getSettingsDeviceHTML();
        String getSettingsWiFiHTML();
        String getSettingsMQTTHTML();
        String getSettingsDateHTML();
        String getSettingsLoggingHTML();
        String getSettingsGarageDoorHTML();

        String getAdminHTML();

        request_device_t parseDeviceBody(String body);
        
        request_wifiAP_t parseWiFiBody(String body);
        std::vector<request_wifiAP_t> parseMultiWiFiBody(String body);

        request_mqtt_t parseMQTTBody(String body);

        request_dateTime_t parseDateBody(String body);

        request_logging_t parseLoggingBody(String body);

        request_garage_door_t parseGarageDoorBody(String body);

    public:
        HttpHandlers(Application *app, Settings *settings, GarageDoor *garageDoor, Statistics *statistics);

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

        void handleGetStatusGlobal();
        void handleGetStatusStatistics();
        void handleGetStatusStatisticsGraph();
        void handleDownloadStatistics();
        void handleDeleteStatistics();

        void handleGetSettingsDevice();
        void handleUpdSettingsDevice();

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

        void handleGetSettingsGarageDoor();
        void handleUpdSettingsGarageDoor();

        void handleGetAdmin();
};

extern HttpHandlers *httpHandlers;

#endif
