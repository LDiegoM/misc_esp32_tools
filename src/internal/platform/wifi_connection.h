#ifndef wifi_connection_h
#define wifi_connection_h

#include <vector>
#include <WiFiMulti.h>

struct wifiAP_t {
    String ssid, password;
};

//struct std::vector<wifiAP_t> m_wifiAPs;

class WiFiConnection {
    private:
        const char *SSID_AP = "misc_esp32_tools";

        WiFiMulti *m_wifiMulti;
        std::vector<wifiAP_t> m_wifiAPs;

    public:
        WiFiConnection(std::vector<wifiAP_t> wifiAPs);

        bool begin();
        bool connect(bool verbose);
        bool beginAP();
        bool isConnected();
        bool isModeAP();
        String getIP();
        String getSSID();
};

#endif
