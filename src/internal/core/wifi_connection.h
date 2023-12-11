#ifndef wifi_connection_h
#define wifi_connection_h

#include <vector>
#include <WiFiMulti.h>
#include <internal/core/logging.h>

struct wifiAP_t {
    String ssid, password;
};

class WiFiConnection {
    private:
        const char *SSID_AP = "esp32_application";

        WiFiMulti *m_wifiMulti = nullptr;
        std::vector<wifiAP_t> m_wifiAPs;
        String m_apSSID = "";

        String getStatusFromInt(uint8_t status);

    public:
        WiFiConnection();
        WiFiConnection(String apSSID);
        WiFiConnection(std::vector<wifiAP_t> wifiAPs);

        bool begin();
        bool connect(bool verbose);
        bool beginAP();
        bool beginAP(String apSSID);
        String getDeviceAPSSID();
        bool isConnected();
        bool isModeAP();
        String getIP();
        String getSSID();
        String getStatus();
};

#endif
