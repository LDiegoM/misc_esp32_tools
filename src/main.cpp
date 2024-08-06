#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <Fonts/FreeSans9pt7b.h>

#include <internal/core/application.h>
#include <internal/garage_door/garage_door.h>
#include <handlers/http.h>

#include <internal/display/display.h>
#include <internal/display/number_set.h>
#include <internal/sensors/sensors.h>

#define BACKGROUND WHITE
#define FORE_COLOR BLACK

#define PIN_BOOT_INDICATOR 15

#define TFT_CS 2 // TFT display cable select pin
#define TFT_DC 4 // TFT display command pin

#define DHTPIN 17
#define DHTTYPE DHT11

Application *app = nullptr;
Settings *settings = nullptr;
GarageDoor *garageDoor = nullptr;

TFT_ILI9163C *tft = nullptr;
Sensors *sensors;
Display *display;

bool isWiFiConnected() {
    if (app == nullptr)
        return false;
    if (app->wifi() == nullptr)
        return false;
    return app->wifi()->isConnected();
}

void setup() {
    Serial.begin(115200);
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

    tft = new TFT_ILI9163C(TFT_CS, TFT_DC);
    tft->begin();
    tft->setFont(&FreeSans9pt7b);

    sensors = new Sensors(DHTPIN, DHTTYPE);
    while (!sensors->begin()) {
        lg->error("Could not load sensors", __FILE__, __LINE__);
        app->bootIndicator()->startErrorBlink();
        return;
    }

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

    mqttHandlers = new MqttHandlers(garageDoor);
    mqttHandlers->begin();

    display = new Display(tft, BACKGROUND, FORE_COLOR, sensors, app->wifi(), app->dateTime());
    display->begin();
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
