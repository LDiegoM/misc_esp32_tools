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
        const char *SSID_AP = "esp32_tools";

        WiFiMulti *m_wifiMulti;
        std::vector<wifiAP_t> m_wifiAPs;

        String getStatusFromInt(uint8_t status);

    public:
        WiFiConnection(std::vector<wifiAP_t> wifiAPs);

        bool begin();
        bool connect(bool verbose);
        bool beginAP();
        bool isConnected();
        bool isModeAP();
        String getIP();
        String getSSID();
        String getStatus();
};

#endif
