//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// JSON Config File on SPIFFS
//

#ifdef ENABLE_JSON
#ifndef ARDUINO_UNO

#include "FS.h"
#include <ArduinoJson.h>

#define BUF_SIZE 1024

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

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, configFile);

  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  // wifi_ssid = doc["ssid"];
  // wifi_pass = doc["password"];
  gTargetTemp = doc["tset"];
  gOvershoot = doc["tband"];
  gP = doc["P"], gI = doc["I"], gD = doc["D"];
  gaP = doc["aP"], gaI = doc["aI"], gaD = doc["aD"];

  return true;
}

bool saveConfig() {
  JsonDocument doc;
  // doc["ssid"] = wifi_ssid;  doc["password"] = wifi_pass;
  doc["tset"] = gTargetTemp;
  doc["tband"] = gOvershoot;
  doc["P"] = gP;
  doc["I"] = gI;
  doc["D"] = gD;
  doc["aP"] = gaP;
  doc["aI"] = gaI;
  doc["aD"] = gaD;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}

void resetConfig() {
  gP = S_P;
  gI = S_I;
  gD = S_D;
  gaP = S_aP;
  gaI = S_aI;
  gaD = S_aD;
  gTargetTemp = S_TSET;
  gOvershoot = S_TBAND;
}

#endif
#endif
