# misc_esp32_tools - About

Miscelanious tools for my home built with ESP32.

It was developed using PaltformIO with Visual Studio Code.

In [this document](./doc/README.md) you'll find the electronic details to assemble this project.

# Features

It's main feature is to alert when my houses garage door is opened for a considerable time, and when dorbell button is pressed.

For future versions I hope to collect these events data for analysis porpuses.

# Getting started

When misc_esp32_tools is running with an empty configuration, it'll not be able to connect to MQTT and date time services. To allow user to configure misc_esp32_tools, it creates a default WiFi access point. You can connect to this WiFi network and access the next [link](http://192.168.4.1) to configure misc_esp32_tools.

After this, when misc_esp32_tools is running connected to a WiFi network and MQTT services, then you can configure it accessing to `http://misc_esp32_tools_ip_address/`.

![Status](./doc/html_status.jpg) ![Administration](./doc/html_admin.jpg)

# MQTT service

I choose [Hive MQ](https://www.hivemq.com/) MQTT service for testing porpuses. [Here](./doc/mqtt_ca_root.crt) you'll find the root ca certificate sor a secure connection to this service.

# MQTT monitoring

For monitoring porpuses I configured a complet IoT dashboard using [Iot MQTT Panel](https://play.google.com/store/apps/details?id=snr.lab.iotmqttpanel.prod) application for Android.

[Here](./doc/IoTMQTTPanel.json) is a JSON file to import in IoT MQTT Panel application to built the default dashboards.

![Dashboard](./doc/IoTMQTTPanel_dashboard.jpg) ![Settings](./doc/IoTMQTTPanel_settings.jpg)

# Configuring and managing using MQTT queues

Miscelanious tools listens from the topic `topic-esp_tools-adm-cmd` to receive different commands using the following json format:
```json
{
    "device_id": "device-id",
    "cmd": "some-command"
}
```
It may also receive plain text commands.

Theese are the available commands:
- `DEVICE_ID`:
    - plain text command. All connected devices will send their device-id to MQTT topic `topic-esp_tools-adm-device`.
- `GET_IP`:
    - plain text command. All connected devices will send their local network ips to MQTT topic `topic-esp_tools-adm-ip`.
    - json format. Only the device wich name is equal to device_id parameter will send its local network ip to MQTT topic `topic-esp_tools-adm-ip`
- `GET_LOCATION`:
    - plain text command. All connected devices will send their configured location (lat, long) to MQTT topic `topic-esp_tools-adm-location`.
    - json format. Only the device wich name is equal to device_id parameter will send its configured location (lat, long) to MQTT topic `topic-esp_tools-adm-location`
- `GET_LOG_SIZE`:
    - json format. Only the device wich name is equal to device_id parameter will send its logging file current size to MQTT topic `topic-esp_tools-adm-logsize`
- `GET_STO_FREE`:
    - json format. Only the device wich name is equal to device_id parameter will send its current storage free size to MQTT topic `topic-esp_tools-adm-lofreestogsize`

The `DEVICE_ID` command will send plain text.

All other commands respond a json with this structure:
```json
{
    "device_id": string,
    "value": string
}
```
# Configuration and management API Endpoints

Miscelanious tools implements API endpoints to allow configuration and management, listening in port 80.

[Here](https://app.swaggerhub.com/apis-docs/LDiegoM/esp_tools/1.0.0) is the Open API Documentation.

# License

Miscelanious tools is an open source project by Diego M. Lopez that is licensed under [MIT](https://opensource.org/licenses/MIT). Diego M. Lopez reserves the right to change the license of future releases.

# Author

- Main idea, development and functional prototype by Diego M. Lopez (ldiegom@gmail.com)

# Changelog

## Unreleased

First functional version of the application.
