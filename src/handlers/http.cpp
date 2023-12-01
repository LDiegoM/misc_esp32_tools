#include <handlers/http.h>

HttpHandlers *httpHandlers = nullptr;

//////////////////// HTTP Handlers
void downloadLogs(void) {
    httpHandlers->handleDownloadLogs();
}
void deleteLogs(void) {
    httpHandlers->handleDeleteLogs();
}
void restart() {
    httpHandlers->handleRestart();
}
void getSettings() {
    httpHandlers->handleGetSettings();
}
void delSettings() {
    httpHandlers->handleDelSettings();
}

void getBootstrapCSS() {
    httpHandlers->handleGetBootstrapCSS();
}
void getBootstrapJS() {
    httpHandlers->handleGetBootstrapJS();
}
void getNotFound() {
    httpHandlers->handleGetNotFound();
}

void getStatus() {
    httpHandlers->handleGetStatus();
}

void getSettingsWiFi() {
    httpHandlers->handleGetSettingsWiFi();
}
void addSettingsWiFi() {
    httpHandlers->handleAddSettingsWiFi();
}
void updSettingsWiFi() {
    httpHandlers->handleUpdSettingsWiFi();
}
void delSettingsWiFi() {
    httpHandlers->handleDelSettingsWiFi();
}

void getSettingsMQTT() {
    httpHandlers->handleGetSettingsMQTT();
}
void updSettingsMQTT() {
    httpHandlers->handleUpdSettingsMQTT();
}
void getSettingsMQTTCert() {
    httpHandlers->handleGetSettingsMQTTCert();
}

void getSettingsDate() {
    httpHandlers->handleGetSettingsDate();
}
void updSettingsDate() {
    httpHandlers->handleUpdSettingsDate();
}

void getSettingsLogging() {
    httpHandlers->handleGetSettingsLogging();
}
void updSettingsLogging() {
    httpHandlers->handleUpdSettingsLogging();
}

void getAdmin() {
    httpHandlers->handleGetAdmin();
}

//////////////////// Constructor
HttpHandlers::HttpHandlers(WiFiConnection *wifi, Storage *storage, Settings *settings, DateTime *dateTime) {
    m_wifi = wifi;
    m_storage = storage;
    m_settings = settings;
    m_dateTime = dateTime;
}

//////////////////// Public methods implementation
bool HttpHandlers::begin() {
    if (!m_settings->isSettingsOK())
        return false;

    m_server = new WebServer(HTTP_PORT);

    defineRoutes();

    m_server->begin();

    return true;
}

void HttpHandlers::loop() {
    m_server->handleClient();
}

/////////// HTTP Handlers
void HttpHandlers::handleDownloadLogs() {
    if (!m_storage->exists(LOGGING_FILE)) {
        m_server->send(404, "text/plain", "not found");
        return;
    }

    File file = m_storage->open(LOGGING_FILE);
    if (!file) {
        m_server->send(500, "text/plain", "fail to open logs file");
        return;
    }
    size_t fileSize = file.size();

    String dataType = "application/octet-stream";

    m_server->sendHeader("Content-Disposition", "inline; filename=logs.txt");

    size_t streamSize = m_server->streamFile(file, dataType);
    if (streamSize != fileSize)
        lg->warn("handleDownloadLogs() - sent different data length than expected", __FILE__, __LINE__,
            lg->newTags()
                ->add("file_size", String(fileSize))
                ->add("sent", String(streamSize))
        );
    
    file.close();
}
bool HttpHandlers::handleDeleteLogs() {
    if (!m_storage->exists(LOGGING_FILE)) {
        m_server->send(204);
        return false;
    }

    bool flgOK = m_storage->remove(LOGGING_FILE);
    if (flgOK) {
        lg->info("logging file was deleted by html handler", __FILE__, __LINE__);
        m_server->send(204);
    } else {
        lg->error("html handler could not delete logging file", __FILE__, __LINE__);
        m_server->send(500, "text/plain", "could not delete logging file");
    }

    return flgOK;
}
void HttpHandlers::handleRestart() {
    m_server->send(200, "text/plain", MSG_OK);
    ESP.restart();
}
void HttpHandlers::handleGetSettings() {
    if (!m_storage->exists(SETTINGS_FILE)) {
        m_server->send(404, "text/plain", "not found");
        return;
    }

    m_server->send(200, "application/json", m_storage->readAll(SETTINGS_FILE));
}
void HttpHandlers::handleDelSettings() {
    if (!m_storage->exists(SETTINGS_FILE)) {
        m_server->send(404, "text/plain", "not found");
        return;
    }

    if (!m_storage->remove(SETTINGS_FILE)) {
        m_server->send(500, "text/plain", ERR_SETTINGS_SAVE_GENERIC);
        return;        
    }

    lg->info("all settings were removed and set to defaults", __FILE__, __LINE__);

    handleRestart();
}

void HttpHandlers::handleGetBootstrapCSS() {
    File file = m_storage->open("/wwwroot/bootstrap.min.css.gz");
    if (!file) {
        m_server->send(404);
        return;
    }
    m_server->streamFile(file, "text/css");
    file.close();
}
void HttpHandlers::handleGetBootstrapJS() {
    File file = LittleFS.open("/wwwroot/bootstrap.bundle.min.js.gz");
    if (!file) {
        m_server->send(404);
        return;
    }
    m_server->streamFile(file, "text/js");
    file.close();
}
void HttpHandlers::handleGetNotFound() {
    String html = m_storage->readAll("/wwwroot/error.html");
    html.replace("{error_description}", "Resource not found");
    m_server->send(404, "text/html", html);
}

void HttpHandlers::handleGetStatus() {
    String html = getHeaderHTML("status");
    html += getStatusHTML();
    html += getFooterHTML("status", "");
    m_server->send(200, "text/html", html);
}

void HttpHandlers::handleGetSettingsWiFi() {
    String html = getHeaderHTML("settings");
    html += getSettingsWiFiHTML();
    html += getFooterHTML("settings", "wifi");
    m_server->send(200, "text/html", html);
}
void HttpHandlers::handleAddSettingsWiFi() {
    String body = m_server->arg("plain");
    if (body.equals("")) {
        m_server->send(400, "text/plain", ERR_WIFI_AP_IS_EMPTY);
        return;
    }

    request_wifiAP_t newWiFiAP = parseWiFiBody(body);
    if (newWiFiAP.ssid.equals("")) {
        m_server->send(400, "text/plain", ERR_WIFI_AP_IS_EMPTY);
        return;
    }

    if (m_settings->ssidExists(newWiFiAP.ssid)) {
        m_server->send(400, "text/plain", ERR_WIFI_AP_EXISTS);
        return;
    }

    m_settings->addWifiAP(newWiFiAP.ssid.c_str(), newWiFiAP.password.c_str());
    if (!m_settings->saveSettings()) {
        m_server->send(500, "text/plain", ERR_SETTINGS_SAVE_GENERIC);
        return;        
    }

    m_server->send(200, "text/plain", MSG_OK);
}
void HttpHandlers::handleUpdSettingsWiFi() {
    String body = m_server->arg("plain");
    if (body.equals("")) {
        m_server->send(400, "text/plain", ERR_WIFI_AP_IS_EMPTY);
        return;
    }

    std::vector<wifiAP_t> aps = parseMultiWiFiBody(body);
    if (aps.size() < 1) {
        m_server->send(400, "text/plain", ERR_WIFI_AP_IS_EMPTY);
        return;
    }

    for (int i = 0; i < aps.size(); i++) {
        if (!m_settings->updWifiAP(aps[i].ssid.c_str(), aps[i].password.c_str())) {
            m_server->send(404, "text/plain", ERR_WIFI_AP_NOT_FOUND);
            return;
        }
    }

    if (!m_settings->saveSettings()) {
        m_server->send(500, "text/plain", ERR_SETTINGS_SAVE_GENERIC);
        return;        
    }

    m_server->send(200, "text/plain", MSG_OK);
}
void HttpHandlers::handleDelSettingsWiFi() {
    String ssid = m_server->arg("ap");
    if (ssid.equals("")) {
        m_server->send(400, "text/plain", ERR_WIFI_AP_IS_EMPTY);
        return;
    }

    if (!m_settings->ssidExists(ssid)) {
        m_server->send(404, "text/plain", ERR_WIFI_AP_NOT_FOUND);
        return;
    }

    m_settings->delWifiAP(ssid.c_str());
    if (!m_settings->saveSettings()) {
        m_server->send(500, "text/plain", ERR_SETTINGS_SAVE_GENERIC);
        return;        
    }

    lg->info("wifi AP deleted", __FILE__, __LINE__, lg->newTags()->add("ap", ssid));

    m_server->send(200, "text/plain", MSG_OK);
}

void HttpHandlers::handleGetSettingsMQTT() {
    String html = getHeaderHTML("settings");
    html += getSettingsMQTTHTML();
    html += getFooterHTML("settings", "mqtt");
    m_server->send(200, "text/html", html);
}
void HttpHandlers::handleUpdSettingsMQTT() {
    String body = m_server->arg("plain");
    if (body.equals("")) {
        m_server->send(400, "text/plain", ERR_MQTT_IS_EMPTY);
        return;
    }

    request_mqtt_t mqttValues = parseMQTTBody(body);
    if (mqttValues.server.equals("")) {
        m_server->send(400, "text/plain", ERR_MQTT_IS_EMPTY);
        return;
    }

    if (mqttValues.password.equals("****"))
        m_settings->setMQTTValues(mqttValues.server, mqttValues.username, mqttValues.port, mqttValues.sendPeriod);
    else
        m_settings->setMQTTValues(mqttValues.server, mqttValues.username, mqttValues.password, mqttValues.port, mqttValues.sendPeriod);

    if (!mqttValues.certData.equals("")) {
        if (!m_settings->setMQTTCertificate(mqttValues.certData)) {
            m_server->send(500, "text/plain", ERR_SETTINGS_SAVE_GENERIC);
            return;        
        }
    }

    if (!m_settings->saveSettings()) {
        m_server->send(500, "text/plain", ERR_SETTINGS_SAVE_GENERIC);
        return;        
    }

    lg->info("mqtt settings updated", __FILE__, __LINE__);

    m_server->send(200, "text/plain", MSG_OK);
}
void HttpHandlers::handleGetSettingsMQTTCert() {
    m_server->send(200, "text/plain", m_settings->getSettings().mqtt.ca_cert);
}

void HttpHandlers::handleGetSettingsDate() {
    String html = getHeaderHTML("settings");
    html += getSettingsDateHTML();
    html += getFooterHTML("settings", "date");
    m_server->send(200, "text/html", html);
}
void HttpHandlers::handleUpdSettingsDate() {
    String body = m_server->arg("plain");
    if (body.equals("")) {
        m_server->send(400, "text/plain", ERR_DATE_IS_EMPTY);
        return;
    }

    request_dateTime_t dateSettings = parseDateBody(body);
    if (dateSettings.server1.equals("") || dateSettings.server2.equals("")) {
        m_server->send(400, "text/plain", ERR_DATE_IS_EMPTY);
        return;
    }

    m_settings->setDateValues(dateSettings.server1, dateSettings.server2, dateSettings.gmtOffset, dateSettings.daylightOffset);

    if (!m_settings->saveSettings()) {
        m_server->send(500, "text/plain", ERR_SETTINGS_SAVE_GENERIC);
        return;        
    }

    m_server->send(200, "text/plain", MSG_OK);
}

void HttpHandlers::handleGetSettingsLogging() {
    String html = getHeaderHTML("settings");
    html += getSettingsLoggingHTML();
    html += getFooterHTML("settings", "logging");
    m_server->send(200, "text/html", html);
}
void HttpHandlers::handleUpdSettingsLogging() {
    String body = m_server->arg("plain");
    if (body.equals("")) {
        m_server->send(400, "text/plain", ERR_LOGGING_IS_EMPTY);
        return;
    }

    lg->debug("upd settings logging", __FILE__, __LINE__, lg->newTags()->add("body", body));
    request_logging_t loggingSettings = parseLoggingBody(body);

    m_settings->setLoggingValues(loggingSettings.level, loggingSettings.refreshPeriod);

    if (!m_settings->saveSettings()) {
        lg->error("fail to save logging settings", __FILE__, __LINE__);
        m_server->send(500, "text/plain", ERR_SETTINGS_SAVE_GENERIC);
        return;        
    }

    lg->debug("logging settings updated", __FILE__, __LINE__);
    m_server->send(200, "text/plain", MSG_OK);
}

void HttpHandlers::handleGetAdmin() {
    String html = getHeaderHTML("admin");
    html += getAdminHTML();
    html += getFooterHTML("admin", "admin");
    m_server->send(200, "text/html", html);
}

//////////////////// Private methods implementation
void HttpHandlers::defineRoutes() {
    m_server->on("/", HTTP_GET, getStatus);

    m_server->on("/logs", HTTP_GET, downloadLogs);
    m_server->on("/logs", HTTP_DELETE, deleteLogs);
    m_server->on("/restart", HTTP_POST, restart);
    m_server->on("/settings", HTTP_GET, getSettings);
    m_server->on("/settings", HTTP_DELETE, delSettings);

    m_server->on("/bootstrap.min.css", HTTP_GET, getBootstrapCSS);
    m_server->on("/bootstrap.bundle.min.js", HTTP_GET, getBootstrapJS);

    m_server->on("/settings/wifi", HTTP_GET, getSettingsWiFi);
    m_server->on("/settings/wifi", HTTP_POST, addSettingsWiFi);
    m_server->on("/settings/wifi", HTTP_PUT, updSettingsWiFi);
    m_server->on("/settings/wifi", HTTP_DELETE, delSettingsWiFi);

    m_server->on("/settings/mqtt", HTTP_GET, getSettingsMQTT);
    m_server->on("/settings/mqtt", HTTP_PUT, updSettingsMQTT);
    m_server->on("/settings/mqtt/cert", HTTP_GET, getSettingsMQTTCert);

    m_server->on("/settings/date", HTTP_GET, getSettingsDate);
    m_server->on("/settings/date", HTTP_PUT, updSettingsDate);

    m_server->on("/settings/logging", HTTP_GET, getSettingsLogging);
    m_server->on("/settings/logging", HTTP_PUT, updSettingsLogging);

    m_server->on("/admin", HTTP_GET, getAdmin);

    m_server->onNotFound(getNotFound);
}

request_wifiAP_t HttpHandlers::parseWiFiBody(String body) {
    request_wifiAP_t newWiFiAP;

    StaticJsonDocument<250> configs;
    DeserializationError error = deserializeJson(configs, body);
    if (error) {
        lg->error("fail to parse wifi settings body", __FILE__, __LINE__,
            lg->newTags()
                ->add("method", "deserializeJson()")
                ->add("error", String(error.c_str()))
        );
        return newWiFiAP;
    }
    JsonObject jsonObj = configs.as<JsonObject>();

    newWiFiAP.ssid = jsonObj["ap"].as<String>();
    newWiFiAP.password = jsonObj["pw"].as<String>();

    return newWiFiAP;
}

std::vector<wifiAP_t> HttpHandlers::parseMultiWiFiBody(String body) {
    std::vector<wifiAP_t> aps;

    StaticJsonDocument<250> configs;
    DeserializationError error = deserializeJson(configs, body);
    if (error) {
        lg->error("fail to parse multi-wifi settings body", __FILE__, __LINE__,
            lg->newTags()
                ->add("method", "deserializeJson()")
                ->add("error", String(error.c_str()))
        );
        return aps;
    }
    JsonObject jsonObj = configs.as<JsonObject>();

    for (int i = 0; i < jsonObj["aps"].size(); i++) {
        wifiAP_t ap;
        ap.ssid = jsonObj["aps"][i]["ap"].as<String>();
        ap.password = jsonObj["aps"][i]["pw"].as<String>();

        aps.push_back(ap);
    }

    return aps;
}

request_mqtt_t HttpHandlers::parseMQTTBody(String body) {
    request_mqtt_t mqttValues;

    StaticJsonDocument<4096> configs;
    DeserializationError error = deserializeJson(configs, body);
    if (error) {
        lg->error("fail to parse MQTT settings body", __FILE__, __LINE__,
            lg->newTags()
                ->add("method", "deserializeJson()")
                ->add("error", String(error.c_str()))
        );
        return mqttValues;
    }
    JsonObject jsonObj = configs.as<JsonObject>();

    mqttValues.server = jsonObj["server"].as<String>();
    mqttValues.username = jsonObj["user"].as<String>();
    mqttValues.password = jsonObj["pw"].as<String>();
    mqttValues.port = jsonObj["port"].as<uint16_t>();
    mqttValues.sendPeriod = jsonObj["send_period"].as<uint16_t>();

    String cert = "";
    for (int i = 0; i < jsonObj["cert"].size(); i++) {
        cert += jsonObj["cert"][i].as<String>() + "\n";
    }
    mqttValues.certData = cert;

    return mqttValues;
}

request_dateTime_t HttpHandlers::parseDateBody(String body) {
    request_dateTime_t dateValues;

    StaticJsonDocument<256> configs;
    DeserializationError error = deserializeJson(configs, body);
    if (error) {
        lg->error("fail to parse date settings body", __FILE__, __LINE__,
            lg->newTags()
                ->add("method", "deserializeJson()")
                ->add("error", String(error.c_str()))
        );
        return dateValues;
    }
    JsonObject jsonObj = configs.as<JsonObject>();

    dateValues.server1 = jsonObj["server1"].as<String>();
    dateValues.server2 = jsonObj["server2"].as<String>();
    dateValues.gmtOffset = jsonObj["gmt_offset"].as<long>();
    dateValues.daylightOffset = jsonObj["daylight_offset"].as<int>();

    return dateValues;
}

request_logging_t HttpHandlers::parseLoggingBody(String body) {
    request_logging_t loggingValues;

    StaticJsonDocument<256> configs;
    DeserializationError error = deserializeJson(configs, body);
    if (error) {
        lg->error("fail to parse logging settings body", __FILE__, __LINE__,
            lg->newTags()
                ->add("method", "deserializeJson()")
                ->add("error", String(error.c_str()))
        );
        return loggingValues;
    }
    JsonObject jsonObj = configs.as<JsonObject>();

    loggingValues.level = jsonObj["level"].as<uint8_t>();
    loggingValues.refreshPeriod = jsonObj["refresh_period"].as<uint16_t>();

    return loggingValues;
}
