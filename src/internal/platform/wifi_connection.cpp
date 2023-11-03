#include <internal/platform/wifi_connection.h>

//////////////////// Constructor
WiFiConnection::WiFiConnection(std::vector<wifiAP_t> wifiAPs) {
    m_wifiAPs = wifiAPs;

    m_wifiMulti = new WiFiMulti();
}

//////////////////// Public methods implementation
bool WiFiConnection::begin() {
    for (int i = 0; i < m_wifiAPs.size(); i++) {
        m_wifiMulti->addAP(m_wifiAPs[i].ssid.c_str(),
                           m_wifiAPs[i].password.c_str());
    }

    if (!connect(true)) {
        // If no wifi ap could connect, then begin default AP to enable configuration.
        return beginAP();
    }
    return true;
}

bool WiFiConnection::connect(bool verbose) {
    if (verbose) {
        Serial.println("WiFi Connect");
    }

    if (m_wifiMulti->run() != WL_CONNECTED) {
        if (verbose)
            Serial.println("FAIL");
        return false;
    }

    if (verbose) {
        Serial.println("OK: " + WiFi.SSID());
        delay(1000);
    }
    return true;
}

bool WiFiConnection::beginAP() {
    WiFi.mode(WIFI_AP);
    if (!WiFi.softAP(SSID_AP)) {
        Serial.println("AP Error");
        delay(1000);
        return false;
    }
    Serial.println("AP: " + String(SSID_AP));
    delay(1000);
    return true;
}

bool WiFiConnection::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiConnection::isModeAP() {
    return WiFi.getMode() == WIFI_AP;
}

String WiFiConnection::getIP() {
    if (WiFi.getMode() == WIFI_AP)
        return WiFi.softAPIP().toString();

    if (!isConnected())
        return "";

    return WiFi.localIP().toString();
}

String WiFiConnection::getSSID() {
    if (WiFi.getMode() == WIFI_AP)
        return SSID_AP;
    
    if (WiFi.status() == WL_CONNECTED)
        return WiFi.SSID();
    
    return "";
}
