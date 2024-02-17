#include <internal/core/application.h>
#include <internal/garage_door/garage_door.hpp>
#include <internal/statistics/statistics.hpp>
#include <handlers/http.hpp>

#define PIN_BOOT_INDICATOR 2

Application *app = nullptr;
Settings *settings = nullptr;
GarageDoor *garageDoor = nullptr;
Sqlite3DB *db = nullptr;
Statistics *st = nullptr;

bool isWiFiConnected() {
    if (app == nullptr)
        return false;
    if (app->wifi() == nullptr)
        return false;
    return app->wifi()->isConnected();
}

void setup() {
    Serial.begin(9600);
    app = new Application("esp_tools", PIN_BOOT_INDICATOR, LOG_LEVEL_DEBUG);
    if (!app->beginStorage()) {
        app->bootIndicator()->startErrorBlink();
        return;
    }

    lg->info("device is starting", __FILE__, __LINE__,
        lg->newTags()
            ->add("app_name", app->name())
            ->add("device_id", app->deviceID())
    );

    garageDoor = new GarageDoor();

    settings = new Settings(app, garageDoor);
    if (!settings->begin()) {
        lg->error("Could not load settings", __FILE__, __LINE__);
        app->bootIndicator()->startErrorBlink();
        return;
    }
    if (!settings->isSettingsOK()) {
        lg->error("Settings are not ok", __FILE__, __LINE__);
        app->bootIndicator()->startErrorBlink();
        return;
    }
    settings_t config = settings->getSettings();
    lg->setLevel(config.logging.level);
    lg->setRefreshPeriod(config.logging.refreshPeriod);
    app->setDeviceID(config.app.deviceID);
    app->setGeoLocation(config.app.geoLocation);
    garageDoor->setDoorOpenWarningTime(config.garageDoor.doorOpenWarningTime);
    garageDoor->setRefreshDoorStatusTime(config.garageDoor.refreshDoorStatusTime);

    app->beginWiFi(config.wifiAPs, app->deviceID());
    if (app->beginDateTime(config.dateTime))
        garageDoor->setDateTime(app->dateTime());
    app->beginMqtt(config.mqtt.connection);

    db = new Sqlite3DB(app->storage());
    st = new Statistics(app->storage(), db, app->dateTime());
    if (!st->begin())
        lg->error("fail to begin statistics", __FILE__, __LINE__);
    else
        garageDoor->setStatistics(st);

    httpHandlers = new HttpHandlers(app, settings, garageDoor, st);
    if (!httpHandlers->begin()) {
        lg->error("Could not start http server", __FILE__, __LINE__);
        app->bootIndicator()->startErrorBlink();
        return;
    }

    mqttHandlers = new MqttHandlers(garageDoor);
    mqttHandlers->begin();

    // At the end of setup function, use boot_indicator to show if wifi is connected to an AP or is in AP_MODE.
    if (app->wifi()->isModeAP()) {
        lg->warn("WiFi in AP mode!", __FILE__, __LINE__,
            lg->newTags()
                ->add("ap_name", app->wifi()->getDeviceAPSSID())
        );
        app->bootIndicator()->startWarningBlink();
    } else {
        lg->debug("WiFi OK", __FILE__, __LINE__);
        app->bootIndicator()->setIndicatorStatusCallback(isWiFiConnected);
    }
}

void loop() {
    if (app != nullptr)
        app->loop();

    if (settings == nullptr || !settings->isSettingsOK())
        return;

    if (httpHandlers != nullptr)
        httpHandlers->loop();
    
    if (garageDoor != nullptr)
        garageDoor->loop();
    
    if (mqttHandlers != nullptr)
        mqttHandlers->loop();
}
