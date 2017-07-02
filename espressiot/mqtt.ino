//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// MQTT client to make this a real IoT-Machine
//

//#define MQTT_DEBUG

#ifdef ENABLE_MQTT

char buf_msg[50];

#include <SPI.h>
#include <Ethernet.h>
#include <WiFiClient.h>

#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

void MQTT_reconnect() {
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("ESPressIoT/config/#", 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

void MQTT_callback(char* topic, byte* payload, unsigned int length) {
  #ifdef MQTT_DEBUG
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] '");
  #endif MQTT_DEBUG
  String msg="";
  for (int i = 0; i < length; i++) {
    #ifdef MQTT_DEBUG
    Serial.print((char)payload[i]);
    #endif MQTT_DEBUG
    msg += (char) payload[i];
  }
  #ifdef MQTT_DEBUG
  Serial.println("'");
  #endif MQTT_DEBUG

  double val = msg.toFloat();
  Serial.println(val);
  
  if (strcmp(topic,"ESPressIoT/config/tset")==0){
    if(val>1e-3) gTargetTemp = val;
  }
  else if (strcmp(topic,"ESPressIoT/config/toggle")==0){
    poweroffMode = (!poweroffMode);
  }

}

void setupMQTT(){
  client.setServer("192.168.1.31", 1883);
  client.setCallback(MQTT_callback);  
}

void loopMQTT() {
 
  if (!client.connected()) {
    MQTT_reconnect();
  }
  client.loop();
  dtostrf(gInputTemp,10,2,buf_msg);
  client.publish("ESPressIoT/temp", buf_msg);
  dtostrf(gOutputPwr,10,2,buf_msg);
  client.publish("ESPressIoT/power", buf_msg);
  dtostrf(gTargetTemp,10,2,buf_msg);
  client.publish("ESPressIoT/tset", buf_msg);
  
}

#endif ENABLE_MQTT
