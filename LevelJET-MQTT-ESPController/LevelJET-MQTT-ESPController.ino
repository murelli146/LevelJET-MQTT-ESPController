//LevelJET-MQTT-ESPController.ino

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

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
#define DATA_LENGTH 12
unsigned char dataBuffer[DATA_LENGTH];
unsigned int bufferIndex = 0;
unsigned long cycleTime = 300000;  // Zykluszeit der MQTT Daten in Millisekunden (5min)
unsigned long previousMillis = 0;  // Speichert den Zeitpunkt des letzten Zyklus

// CRC-16/BUYPASS Lookup-Tabelle
uint16_t crc_table[256] = {
  0x0000,
  0x8005,
  0x800f,
  0x000a,
  0x801b,
  0x001e,
  0x0014,
  0x8011,
  0x8033,
  0x0036,
  0x003c,
  0x8039,
  0x0028,
  0x802d,
  0x8027,
  0x0022,
  0x8063,
  0x0066,
  0x006c,
  0x8069,
  0x0078,
  0x807d,
  0x8077,
  0x0072,
  0x0050,
  0x8055,
  0x805f,
  0x005a,
  0x804b,
  0x004e,
  0x0044,
  0x8041,
  0x80c3,
  0x00c6,
  0x00cc,
  0x80c9,
  0x00d8,
  0x80dd,
  0x80d7,
  0x00d2,
  0x00f0,
  0x80f5,
  0x80ff,
  0x00fa,
  0x80eb,
  0x00ee,
  0x00e4,
  0x80e1,
  0x00a0,
  0x80a5,
  0x80af,
  0x00aa,
  0x80bb,
  0x00be,
  0x00b4,
  0x80b1,
  0x8093,
  0x0096,
  0x009c,
  0x8099,
  0x0088,
  0x808d,
  0x8087,
  0x0082,
  0x8183,
  0x0186,
  0x018c,
  0x8189,
  0x0198,
  0x819d,
  0x8197,
  0x0192,
  0x01b0,
  0x81b5,
  0x81bf,
  0x01ba,
  0x81ab,
  0x01ae,
  0x01a4,
  0x81a1,
  0x01e0,
  0x81e5,
  0x81ef,
  0x01ea,
  0x81fb,
  0x01fe,
  0x01f4,
  0x81f1,
  0x81d3,
  0x01d6,
  0x01dc,
  0x81d9,
  0x01c8,
  0x81cd,
  0x81c7,
  0x01c2,
  0x0140,
  0x8145,
  0x814f,
  0x014a,
  0x815b,
  0x015e,
  0x0154,
  0x8151,
  0x8173,
  0x0176,
  0x017c,
  0x8179,
  0x0168,
  0x816d,
  0x8167,
  0x0162,
  0x8123,
  0x0126,
  0x012c,
  0x8129,
  0x0138,
  0x813d,
  0x8137,
  0x0132,
  0x0110,
  0x8115,
  0x811f,
  0x011a,
  0x810b,
  0x010e,
  0x0104,
  0x8101,
  0x8303,
  0x0306,
  0x030c,
  0x8309,
  0x0318,
  0x831d,
  0x8317,
  0x0312,
  0x0330,
  0x8335,
  0x833f,
  0x033a,
  0x832b,
  0x032e,
  0x0324,
  0x8321,
  0x0360,
  0x8365,
  0x836f,
  0x036a,
  0x837b,
  0x037e,
  0x0374,
  0x8371,
  0x8353,
  0x0356,
  0x035c,
  0x8359,
  0x0348,
  0x834d,
  0x8347,
  0x0342,
  0x03c0,
  0x83c5,
  0x83cf,
  0x03ca,
  0x83db,
  0x03de,
  0x03d4,
  0x83d1,
  0x83f3,
  0x03f6,
  0x03fc,
  0x83f9,
  0x03e8,
  0x83ed,
  0x83e7,
  0x03e2,
  0x83a3,
  0x03a6,
  0x03ac,
  0x83a9,
  0x03b8,
  0x83bd,
  0x83b7,
  0x03b2,
  0x0390,
  0x8395,
  0x839f,
  0x039a,
  0x838b,
  0x038e,
  0x0384,
  0x8381,
  0x0280,
  0x8285,
  0x828f,
  0x028a,
  0x829b,
  0x029e,
  0x0294,
  0x8291,
  0x82b3,
  0x02b6,
  0x02bc,
  0x82b9,
  0x02a8,
  0x82ad,
  0x82a7,
  0x02a2,
  0x82e3,
  0x02e6,
  0x02ec,
  0x82e9,
  0x02f8,
  0x82fd,
  0x82f7,
  0x02f2,
  0x02d0,
  0x82d5,
  0x82df,
  0x02da,
  0x82cb,
  0x02ce,
  0x02c4,
  0x82c1,
  0x8243,
  0x0246,
  0x024c,
  0x8249,
  0x0258,
  0x825d,
  0x8257,
  0x0252,
  0x0270,
  0x8275,
  0x827f,
  0x027a,
  0x826b,
  0x026e,
  0x0264,
  0x8261,
  0x0220,
  0x8225,
  0x822f,
  0x022a,
  0x823b,
  0x023e,
  0x0234,
  0x8231,
  0x8213,
  0x0216,
  0x021c,
  0x8219,
  0x0208,
  0x820d,
  0x8207,
  0x0202,
};

void setup() {

  setupOTA();
  Serial.begin(19200, SERIAL_8N1);
  DEBUG_INIT(115200);
  setup_wifi();
  setup_mqtt();

  // Abonnieren der Topics
  client.subscribe("LevelJET/cmd/#");


  DEBUG_PRINTLN("LevelJet Schnittstelle initialisiert");
}

void loop() {

  ArduinoOTA.handle();
  // MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // LevelJET_______________________________
  if (Serial.available() > 0) {
    unsigned char incomingByte = Serial.read();
    dataBuffer[bufferIndex++] = incomingByte;

    // Wenn der Puffer voll ist
    if (bufferIndex == DATA_LENGTH) {
      // Verarbeitung der Daten
      processReceivedData(dataBuffer);
      bufferIndex = 0;  // Zurücksetzen des Buffer-Index für den nächsten Datenblock
    }
  }
}

uint16_t crc16(uint8_t* data, size_t length) {
  uint16_t crc = 0x0000;  // Startwert für CRC-16/BUYPASS ist 0x0000
  for (size_t i = 0; i < length; ++i) {
    uint8_t index = (crc >> 8) ^ data[i];  // Berechnen des Index für die Lookup-Tabelle
    crc = (crc << 8) ^ crc_table[index];
  }
  return crc;
}

void processReceivedData(const uint8_t* data) {
  DEBUG_PRINTLN("Empfangene Daten:");
  for (int i = 0; i < DATA_LENGTH; ++i) {
    if (data[i] < 0x10) {
      DEBUG_PRINT("0");  // Fügt eine führende Null hinzu, wenn der Wert kleiner als 0x10 ist
    }
    DEBUG_PRINT_HEX(data[i]);
    DEBUG_PRINT(" ");  // Fügt ein Leerzeichen zwischen den Bytes für bessere Lesbarkeit hinzu
  }
  DEBUG_PRINTLN();  // Fügt einen Zeilenumbruch nach der vollständigen Ausgabe hinzu


  unsigned long currentMillis = millis();
  // Berechnen des CRC über die ersten 10 Bytes der Daten
  uint16_t calculatedCRC = crc16((uint8_t*)data, 10);  // Cast zu non-const, da unsere Funktion non-const erwartet

  // Extrahieren des empfangenen CRC (angenommen, es ist in Big-Endian im Datenpaket)
  uint16_t receivedCRC = (uint16_t)(data[11] << 8 | data[10]);

  DEBUG_PRINT("Empangenser CRC: ");
  DEBUG_PRINTLN_HEX(receivedCRC);
  DEBUG_PRINT("Berechneter CRC: ");
  DEBUG_PRINTLN_HEX(calculatedCRC);

  // Vergleichen des berechneten CRC mit dem empfangenen CRC
  if (calculatedCRC == receivedCRC) {
    DEBUG_PRINTLN("CRC check passed");
    // Weiterverarbeitung der Daten hier
    // Interpretation der Daten gemäß Protokoll
    unsigned int geraetekennung = (unsigned int)(data[0] | data[1] << 8);
    unsigned int distanz = (unsigned int)(data[2] | data[3] << 8);
    unsigned int fuellhoehe = (unsigned int)(data[4] | data[5] << 8);
    unsigned int liter = (unsigned int)(data[6] | data[7] << 8);
    unsigned char inhaltProzent = data[8];
    unsigned char zustandAusgaenge = data[9];

    // Masken für die Bits, die die Ausgänge repräsentieren
    unsigned char ausgang1Maske = 0b00010000;  // Bit 4
    unsigned char ausgang2Maske = 0b00100000;  // Bit 5

    // Extrahieren der Zustände der Ausgänge
    bool ausgang1 = zustandAusgaenge & ausgang1Maske;
    bool ausgang2 = zustandAusgaenge & ausgang2Maske;


    if (currentMillis - previousMillis >= cycleTime) {
      StaticJsonDocument<512> doc;  // Umstellung auf JSON (doc)
      char buffer[512];             // Umstellung auf  JSON  (doc)

      // MQTT publish
      doc["Kennung"] = geraetekennung;
      doc["Distanz"] = distanz;
      doc["Fuellhoehe"] = fuellhoehe;
      doc["Liter"] = liter * 10;
      doc["Inhalt_Prozent"] = inhaltProzent;
      doc["Ausgang1"] = ausgang1;
      doc["Ausgang2"] = ausgang2;
      doc["sendezyklus"] = cycleTime / 1000;

      /* Umstellung auf JSON (doc)
      client.publish("LevelJET/status/Kennung", String(geraetekennung).c_str());
      client.publish("LevelJET/status/Distanz", String(distanz).c_str());
      client.publish("LevelJET/status/Füllhöhe", String(fuellhoehe).c_str());
      client.publish("LevelJET/status/Liter", String(liter * 10).c_str());
      client.publish("LevelJET/status/Inhalt_Prozent", String(inhaltProzent).c_str());
      client.publish("LevelJET/status/Ausgang1", String(ausgang1).c_str());
      client.publish("LevelJET/status/Ausgang2", String(ausgang2).c_str());
      client.publish("LevelJET/status/sendezyklus", String(cycleTime / 1000).c_str());
      */

      size_t len = serializeJson(doc, buffer, sizeof(buffer));
      client.publish("LevelJET/status", buffer, len);
      doc.clear();  // JSON-Dokument für den nächsten Abschnitt leeren

      previousMillis = currentMillis;
    }
    // Debug-Ausgabe auf Serial1
    DEBUG_PRINT("Gerätekennung: ");
    DEBUG_PRINTLN(geraetekennung);
    DEBUG_PRINT("Distanz: ");
    DEBUG_PRINTLN(distanz);
    DEBUG_PRINT("Füllhöhe: ");
    DEBUG_PRINTLN(fuellhoehe);
    DEBUG_PRINT("Liter: ");
    DEBUG_PRINTLN(liter * 10);
    DEBUG_PRINT("Inhalt in Prozent: ");
    DEBUG_PRINTLN(inhaltProzent);
    DEBUG_PRINT("Ausgang 1: ");
    DEBUG_PRINTLN(ausgang1 ? "1" : "0");
    DEBUG_PRINT("Ausgang 2: ");
    DEBUG_PRINTLN(ausgang2 ? "1" : "0");

  } else {
    DEBUG_PRINT("CRC check failed: Calculated CRC = 0x");
    DEBUG_PRINT_HEX(calculatedCRC);
    DEBUG_PRINT(", Received CRC = 0x");
    DEBUG_PRINTLN_HEX(receivedCRC);
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  // Erstelle einen String für das Thema und reserviere Speicher
  String topicStr = String(topic);
  topicStr.reserve(100);  // Angenommene maximale Länge des Themas

  // Erstelle einen String für die Nachricht und reserviere Speicher
  String messageStr;
  messageStr.reserve(100);  // Angenommene maximale Länge der Nachricht
                            // Konvertiere die Nachricht von byte* zu String
  for (int i = 0; i < length; i++) {
    messageStr += (char)message[i];
  }

  DEBUG_PRINT("Topic: ");
  DEBUG_PRINTLN(topicStr);

  // Verarbeite das Thema und die Nachricht
  if (topicStr == "LevelJET/cmd/sendezyklus") {
    int value = messageStr.toInt();
    if (value >= 10 && value <= 3600) {
      cycleTime = value * 1000;
      DEBUG_PRINT("Neuer Sendezyklus: ");
      DEBUG_PRINTLN(cycleTime / 1000);
      client.publish("LevelJET/status/sendezyklus", String(cycleTime / 1000).c_str());
    }
  }
}
