#include <internal/core/boot_indicator.h>
#include <internal/core/date_time.h>
#include <internal/core/storage.h>
#include <internal/core/logging.h>
#include <internal/core/timer.h>
#include <internal/core/wifi_connection.h>
#include <internal/settings/settings.h>

#include <handlers/http.h>

#define PIN_BOOT_INDICATOR 2

BootIndicator *m_bootIndicator;
Storage *storage;
Settings *settings;
WiFiConnection *wifi;
DateTime *dateTime;

bool isWiFiConnected() {
    return wifi->isConnected();
}

void setup() {
    m_bootIndicator = new BootIndicator(PIN_BOOT_INDICATOR, false);

    storage = new Storage();
    while (!storage->begin()) {
        Serial.print(".");
    }

    lg = new Logging(LOG_LEVEL_DEBUG, storage);
    lg->info("device is starting", __FILE__, __LINE__);

    settings = new Settings(storage);
    if (!settings->begin()) {
        lg->error("Could not load settings", __FILE__, __LINE__);
        m_bootIndicator->startErrorBlink();
        return;
    }
    if (!settings->isSettingsOK()) {
        lg->error("Settings are not ok", __FILE__, __LINE__);
        m_bootIndicator->startErrorBlink();
        return;
    }
    if (settings->getSettings().wifiAPs.size() < 1)
        settings->addWifiAP("Fibertel WiFi349 2.4GHz", "");
    settings_t config = settings->getSettings();
    lg->setLevel(config.logging.level);
    lg->setRefreshPeriod(config.logging.refreshPeriod);

    wifi = new WiFiConnection(config.wifiAPs);
    wifi->begin();

    dateTime = new DateTime(config.dateTime);
    dateTime->begin();
    lg->setDateTime(dateTime);

    httpHandlers = new HttpHandlers(wifi, storage, settings, dateTime);
    if (!httpHandlers->begin()) {
        lg->error("Could not start http server", __FILE__, __LINE__);
        m_bootIndicator->startErrorBlink();
        return;
    }

    if (wifi->isModeAP()) {
        lg->warn("WiFi in AP mode!", __FILE__, __LINE__);
        m_bootIndicator->startWarningBlink();
    } else {
        lg->debug("WiFi OK", __FILE__, __LINE__);
        m_bootIndicator->setIndicatorStatusCallback(isWiFiConnected);
    }
}

void loop() {
    if (m_bootIndicator != nullptr)
        m_bootIndicator->loop();

    if (!settings->isSettingsOK())
        return;

    httpHandlers->loop();
    lg->loop();
}
