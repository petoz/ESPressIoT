//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// Web Server with Options and Plotter
//

#ifdef ENABLE_HTTP

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
IPAddress apIP(10, 0, 0, 1);
IPAddress netMsk(255, 0, 0, 0);

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void handleRoot() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"5\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n<title>EspressIoT</title></head><h1>EspressIoT</h1>\n";
  message += "Measured Temperature: " + String(gInputTemp) + "<br/>\n";
  message += "Target Temperature: " + String(gTargetTemp) + "<br/>\n";
  message += "Heater Power: " + String(gOutputPwr) + "<br/>\n";
  message += "\n";
  if(poweroffMode) message += "<a href=\"./toggleheater\"><button style=\"background-color:#FF0000\">Toggle Heater</button></a><br/>\n";
  else message += "<a href=\"./toggleheater\"><button style=\"background-color:#00FF00\">Toggle Heater</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"./config\"><button>Settings</button></a><br/>\n";
  server.send(200, "text/html", message);
}

void handleConfig() {
  String message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
  message += "<form action=\"set_config\">\nTarget Temperature:<br>\n";
  message += "<input type=\"text\" name=\"tset\" value=\"" + String(gTargetTemp) +"\"><br>\n";
  message += "normal PID:<br>\n P <input type=\"text\" name=\"pgain\" value=\"" + String(gP) + "\"><br/>\n";
  message += "I <input type=\"text\" name=\"igain\" value=\"" + String(gI) + "\"><br/>\n";
  message += "D <input type=\"text\" name=\"dgain\" value=\"" + String(gD) + "\"><br><br>\n";
  message += "adaptive PID:<br>\n P <input type=\"text\" name=\"apgain\" value=\"" + String(gaP) + "\"><br/>\n";
  message += "I <input type=\"text\" name=\"aigain\" value=\"" + String(gaI) + "\"><br/>\n";
  message += "D <input type=\"text\" name=\"adgain\" value=\"" + String(gaD) + "\"><br><br>\n";
  message += "<input type=\"submit\" value=\"Submit\">\n</form>";
  message += "<hr/>";
  message += "<a href=\"./loadconf\"><button>Load Config</button></a><br/>\n";
  message += "<a href=\"./saveconf\"><button>Save Config</button></a><br/>\n";
  message += "<hr/>";
  message += "<a href=\"/\"><button>Back</button></a><br/>\n";
  server.send(200, "text/html", message);
}

void handleSetConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if(server.argName(i)=="tset") {
          message += "new tset: " + server.arg ( i ) + "<br/>\n";
          gTargetTemp = ( (server.arg(i)).toFloat() );
    }
    else if(server.argName(i)=="pgain") {
          message += "new pgain: " + server.arg ( i ) + "<br/>\n";
          gP = ( (server.arg(i)).toFloat() );
    }
    else if(server.argName(i)=="igain") {
          message += "new igain: " + server.arg ( i ) + "<br/>\n";
          gI = ( (server.arg(i)).toFloat() );
    }
    else if(server.argName(i)=="dgain") {
          message += "new pgain: " + server.arg ( i ) + "<br/>\n";
          gD = ( (server.arg(i)).toFloat() );
    }
    else if(server.argName(i)=="apgain") {
          message += "new pgain: " + server.arg ( i ) + "<br/>\n";
          gaP = ( (server.arg(i)).toFloat() );
    }
    else if(server.argName(i)=="aigain") {
          message += "new igain: " + server.arg ( i ) + "<br/>\n";
          gaI = ( (server.arg(i)).toFloat() );
    }
    else if(server.argName(i)=="adgain") {
          message += "new pgain: " + server.arg ( i ) + "<br/>\n";
          gaD = ( (server.arg(i)).toFloat() );
    }
    
  }
  server.send(200, "text/html", message);
  
}

void handleLoadConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if(loadConfig()) message += "<h1>Configuration loaded !</h1>\n";
  else message += "<h1>Error loading configuration !</h1>\n";
  server.send(200, "text/html", message);
}

void handleSaveConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if(saveConfig()) message += "<h1>Configuration saved !</h1>\n";
  else message += "<h1>Error saving configuration !</h1>\n";
  server.send(200, "text/html", message);
}

void handleToggleHeater() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  poweroffMode = (!poweroffMode);
  server.send(200, "text/html", message);
}

void setupWifiSrv() {
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/loadconf", handleLoadConfig);
  server.on("/saveconf", handleSaveConfig);
  server.on("/set_config", handleSetConfig);
  server.on("/toggleheater", handleToggleHeater);
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println("HTTP server started");
}

void loopWifiSrv() {
  server.handleClient();
}

#endif

