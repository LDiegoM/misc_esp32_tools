#include <internal/core/application.h>
#include <internal/audio/doorbell.h>
#include <handlers/http.h>

#define PIN_BOOT_INDICATOR 2

Application *app = nullptr;
Settings *settings = nullptr;
Doorbell *doorbell = nullptr;

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

    settings = new Settings(app);
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

    app->beginWiFi(config.wifiAPs, app->deviceID());
    app->beginDateTime(config.dateTime);
    app->beginMqtt(config.mqtt.connection);

    httpHandlers = new HttpHandlers(app, settings);
    if (!httpHandlers->begin()) {
        lg->error("Could not start http server", __FILE__, __LINE__);
        app->bootIndicator()->startErrorBlink();
        return;
    }

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

    doorbell = new Doorbell();
    mqttHandlers = new MqttHandlers(doorbell);
    mqttHandlers->begin();
}

void loop() {
    if (app != nullptr)
        app->loop();

    if (settings == nullptr || !settings->isSettingsOK())
        return;

    if (httpHandlers != nullptr)
        httpHandlers->loop();
    
    if (doorbell != nullptr)
        doorbell->loop();
    
    if (mqttHandlers != nullptr)
        mqttHandlers->loop();
}
