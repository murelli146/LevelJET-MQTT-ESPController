//LevelJET-MQTT-ESPController

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#include "setup.h"
#include "config.h"

// OTA Firmwareupdate
const char* OTA_HOSTNAME = "LevelJET";
const char* OTA_PASSWORD = "1234";

// WLAN
const char* ssid = my_ssid;          //const char* ssid = "ihre_ssid";  !!!
const char* password = my_password;  //const char* password = "ihr_Passwort"; !!!

// MQTT
const char* mqtt_Server = my_mqtt_Server;  //const char* mqtt_Server = "192.168.0.77"; !!!
const int mqttPort = 1883;
const char* mqttUser = "Ihr_MQTT_Benutzername";  // Optional
const char* mqttPassword = "Ihr_MQTT_Passwort";  // Optional
const char* maintopic = "ComfoAir/";
WiFiClient espClient;
PubSubClient client(espClient);

// LevelJET

#define DATA_LENGTH 12  // Länge des Datenblocks

void setup() {
  // put your setup code here, to run once:
  setupOTA();
  Serial.begin(19200, SERIAL_8N1);
  Serial1.begin(115200);
  setup_wifi();
  setup_mqtt();

  Serial1.println("LevelJet Schnittstelle initialisiert");
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
  // MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // LevelJET_______________________________
  static unsigned char dataBuffer[DATA_LENGTH];
  static int bytesReceived = 0;

  // Prüfen, ob Daten verfügbar sind
  if (Serial.available()) {
    // Lesen Sie ein Byte vom seriellen Port
    int incomingByte = Serial.read();
    // Fügen Sie das gelesene Byte zum Buffer hinzu, wenn wir noch nicht alle Daten haben
    if (bytesReceived < DATA_LENGTH) {
      dataBuffer[bytesReceived++] = (unsigned char)incomingByte;
    }

    // Überprüfen, ob wir einen vollständigen Datenblock erhalten haben
    if (bytesReceived == DATA_LENGTH) {
      // Verarbeiten Sie hier die Daten
      // Zum Beispiel: Ausgabe der Daten zur Überprüfung
      for (int i = 0; i < DATA_LENGTH; i++) {
        Serial1.print(dataBuffer[i], HEX);
        Serial1.print(" ");
      }
      Serial1.println();

      // Buffer zurücksetzen für den nächsten Datenblock
      bytesReceived = 0;
    }
  }

  // Führen Sie hier weitere Aufgaben aus, die regelmäßig geprüft werden müssen
  // Zum Beispiel: Überprüfung von MQTT-Nachrichten, Sensorabfragen, etc.



  /*
  static unsigned char dataBuffer[DATA_LENGTH];
  static int bytesReceived = 0;

  while (Serial.available()) {
    // Lesen Sie ein Byte vom LevelJET
    int incomingByte = Serial.read();
    // Fügen Sie das gelesene Byte zum Buffer hinzu, wenn wir noch nicht alle Daten haben
    if (bytesReceived < DATA_LENGTH) {
      dataBuffer[bytesReceived++] = (unsigned char)incomingByte;
    }

    // Überprüfen, ob wir einen vollständigen Datenblock erhalten haben
    if (bytesReceived == DATA_LENGTH) {
      // Verarbeiten Sie hier die Daten
      // Zum Beispiel: Senden der Daten über MQTT
      sendLevelJetDataOverMQTT(dataBuffer);

      // Buffer zurücksetzen für den nächsten Datenblock
      bytesReceived = 0;
    }
  }
  */
}

void sendLevelJetDataOverMQTT(unsigned char* data) {
  // Implementieren Sie die Logik zum Senden der Daten über MQTT
  // Beispiel: MQTTClient.publish("LevelJET/Distanz", String(data[2] | data[3] << 8));
}
