//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// JSON Config File on SPIFFS
//

#include <ArduinoJson.h>
#include "FS.h"

#define BUF_SIZE 256

const char* wifi_ssid = "";
const char* wifi_pass = "";

bool prepareFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return false;
  }
  return true;
}

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<BUF_SIZE> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  wifi_ssid = json["ssid"];
  wifi_pass = json["password"];
  gTargetTemp = json["tset"];
  gOvershoot = json["tband"];
  gP = json["P"], gI = json["I"], gD = json["D"];
  gaP = json["aP"], gaI = json["aI"], gaD = json["aD"];

  Serial.print("Loaded WIFI SSID: ");
  Serial.println(wifi_ssid);
  Serial.print("Loaded WIFI Password: ");
  Serial.println(wifi_pass);
  return true;
}

bool saveConfig() {
  StaticJsonBuffer<BUF_SIZE> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ssid"] = wifi_ssid;  json["password"] = wifi_pass;
  json["tset"] = gTargetTemp;  json["tband"] = gOvershoot;
  json["P"] = gP, json["I"] = gI, json["D"] = gD;
  json["aP"] = gaP, json["aI"] = gaI, json["aD"] = gaD;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}

