#include <handlers/http.h>

String HttpHandlers::getHeaderHTML(String section) {
    String header = m_storage->readAll("/wwwroot/header.html");

    header.replace("{active_status}", (section.equals("status") ? " active" : ""));
    header.replace("{active_settings}", (section.equals("settings") ? " active" : ""));
    header.replace("{active_admin}", (section.equals("admin") ? " active" : ""));

    if (!m_wifi->isModeAP() /*&& m_mqtt->isConnected()*/) {
        header.replace("/bootstrap.min.css", BOOTSTRAP_CSS);
        header.replace("/bootstrap.bundle.min.js", BOOTSTRAP_JS);
    }

    return header;
}

String HttpHandlers::getFooterHTML(String page, String section) {
    String footer = m_storage->readAll("/wwwroot/footer.html");
    String js = "";
    js += "<script>";
    js += m_storage->readAll("/wwwroot/utils.js");
    js += "\n";
    if (!page.equals("") && !section.equals(""))
        js += m_storage->readAll((String("/wwwroot/") + page + "/" + section + ".js").c_str());
    js += "</script>";
    footer.replace("<!--{utils.js}-->", js);

    return footer;
}

String HttpHandlers::getStatusHTML() {
    String html = m_storage->readAll("/wwwroot/status/status.html");
    if (!m_dateTime->refresh()) {
        html.replace("{date_time}", "ERROR refreshing date_time");
    } else {
        html.replace("{date_time}", m_dateTime->toString());
    }

    if (m_wifi->isModeAP()) {
        html.replace("{wifi_connected}", DISCONNECTED);
        html.replace("{ssid}", "AP: " + m_wifi->getSSID());
        html.replace("{ip}", "IP: " + m_wifi->getIP());
    } else if (m_wifi->isConnected()) {
        html.replace("{wifi_connected}", CONNECTED);
        html.replace("{ssid}", "SSID: " + m_wifi->getSSID());
        html.replace("{ip}", "IP: " + m_wifi->getIP());
    } else {
        html.replace("{wifi_connected}", DISCONNECTED);
        html.replace("{ssid}", "");
        html.replace("{ip}", "");
    }

    html.replace("{free_storage}", m_storage->getFree());
    html.replace("{free_mem}", String((float) ESP.getFreeHeap() / 1024) + " kb");

    /*if (m_mqtt->isConnected())
        html.replace("{mqtt_connected}", CONNECTED);
    else*/
        html.replace("{mqtt_connected}", DISCONNECTED);

    return html;
}

String HttpHandlers::getSettingsWiFiHTML() {
    String htmlUpdate = "";
    for (int i = 0; i < m_settings->getSettings().wifiAPs.size(); i++) {
        String htmlAP = m_storage->readAll("/wwwroot/settings/wifi_update_ap.html");
        htmlAP.replace("{ap_name}", m_settings->getSettings().wifiAPs[i].ssid);
        htmlAP += "\n";
        htmlUpdate += htmlAP;
    }

    String html = m_storage->readAll("/wwwroot/settings/wifi.html");
    html.replace("<!--{wifi_update_ap.html}-->", htmlUpdate);
    return html;
}

String HttpHandlers::getSettingsMQTTHTML() {
    String html = m_storage->readAll("/wwwroot/settings/mqtt.html");

    settings_t settings = m_settings->getSettings();
    html.replace("{server}", settings.mqtt.server);
    html.replace("{user}", settings.mqtt.username);
    html.replace("{port}", String(settings.mqtt.port));
    html.replace("{sendPeriod}", String(settings.mqtt.sendPeriod));
    html.replace("{certificate}", "");

    return html;
}

String HttpHandlers::getSettingsDateHTML() {
    String html = m_storage->readAll("/wwwroot/settings/date.html");

    settings_t settings = m_settings->getSettings();
    html.replace("{server1}", String(settings.dateTime.server1));
    html.replace("{server2}", String(settings.dateTime.server2));
    html.replace("{gmtOffset}", String(settings.dateTime.gmtOffset));
    html.replace("{daylightOffset}", String(settings.dateTime.daylightOffset));

    return html;
}

String HttpHandlers::getSettingsLoggingHTML() {
    String html = m_storage->readAll("/wwwroot/settings/logging.html");

    settings_t settings = m_settings->getSettings();
    html.replace("{active_debug}", settings.logging.level == LOG_LEVEL_DEBUG ? "selected" : "");
    html.replace("{active_info}", settings.logging.level == LOG_LEVEL_INFO ? "selected" : "");
    html.replace("{active_warning}", settings.logging.level == LOG_LEVEL_WARNING ? "selected" : "");
    html.replace("{active_error}", settings.logging.level == LOG_LEVEL_ERROR ? "selected" : "");
    html.replace("{refresh_period}", String(settings.logging.refreshPeriod));

    return html;
}


String HttpHandlers::getAdminHTML() {
    String html = m_storage->readAll("/wwwroot/admin/admin.html");

    html.replace("{logs_size}", lg->logSize());

    return html;
}
