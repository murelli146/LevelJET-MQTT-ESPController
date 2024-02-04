# LevelJET-MQTT-ESPController

Dieses Projekt ermöglicht die Anbindung eines LevelJET Füllstandmessgeräts an ein MQTT-Netzwerk mittels eines ESP8266 Mikrocontrollers. Es liest die Daten vom LevelJET-Gerät über eine serielle Verbindung und veröffentlicht die Messwerte über MQTT. Dies ermöglicht die Integration in Smart Home Systeme oder andere IoT-Anwendungen.

## Installation

### Voraussetzungen

- Ein ESP8266 Mikrocontroller.
- LevelJET Füllstandmessgerät.
- Arduino IDE oder eine ähnliche Entwicklungsumgebung.

### Setup

1. Laden Sie den Code auf Ihren ESP8266 Mikrocontroller hoch, nachdem Sie ihn entsprechend Ihrer Netzwerk- und MQTT-Broker-Konfiguration angepasst haben.
2. Stellen Sie sicher, dass Ihr LevelJET-Gerät korrekt angeschlossen ist und Daten sendet.

## Konfiguration

Im Code müssen Sie die folgenden Einstellungen anpassen:

- `ssid` und `password` für Ihre WLAN-Verbindung.
- `mqtt_Server`, `mqttPort`, `mqttUser`, und `mqttPassword` für die Verbindung zu Ihrem MQTT-Broker.
- `OTA_HOSTNAME` und `OTA_PASSWORD` für OTA-Updates.

Siehe `config.h` für weitere Konfigurationsmöglichkeiten.

## Benutzung

Nach der erfolgreichen Installation und Konfiguration verbindet sich der ESP8266 mit Ihrem WLAN und dem MQTT-Broker. Der Controller abonniert MQTT-Topics für Befehle und veröffentlicht Daten von Ihrem LevelJET-Gerät unter `LevelJET/status/`.

Sie können die Zykluszeit für das Senden der Daten über das Topic `LevelJET/cmd/sendezyklus` anpassen.

## Protokollbeschreibung

Die Kommunikation zwischen dem LevelJET-Gerät und dem ESP8266 erfolgt über eine serielle Schnittstelle mit folgenden Parametern:

- Geschwindigkeit: 19200 Baud
- Datenbits: 8
- Parität: Keine
- Stopbits: 1

Ein Datenblock umfasst 12 Bytes, davon 10 Datenbytes und zwei CRC-Bytes. Die Zuordnung der Datenbytes ist wie folgt:

- 0-1: Gerätekennung
- 2-3: Distanz
- 4-5: Füllhöhe
- 6-7: Liter
- 8: Inhalt in Prozent
- 9: Zustand der Ausgänge
- 10-11: CRC

## Beitragen

Fühlen Sie sich frei, zum Projekt beizutragen. Jede Hilfe bei der Entwicklung, Dokumentation oder beim Testen ist willkommen. Bitte erstellen Sie Pull Requests für Ihre Beiträge.

## Lizenz

Dieses Projekt ist unter der MIT-Lizenz lizenziert. Siehe die LICENSE-Datei im Repository für die vollständigen Lizenztexte.
