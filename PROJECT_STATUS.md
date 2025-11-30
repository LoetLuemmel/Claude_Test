# Projekt Status: ESP32 Matter Light

**Letzte Aktualisierung**: 2025-11-30 (Erfolgreich auf ESP32 portiert und mit Apple Home gepairt)
**Claude Code Session Status**

---

## ✅ Abgeschlossene Aufgaben

### 1. Projektstruktur erstellt
- [x] Haupt-CMakeLists.txt mit Matter-Integration
- [x] Partitionstabelle (partitions.csv) für 4MB Flash
- [x] sdkconfig.defaults für ESP32-C3 Konfiguration
- [x] main/CMakeLists.txt für Component-Build

### 2. Hauptcode implementiert
- [x] main/app_main.cpp - Vollständige Matter Light Implementierung
  - Matter Node Setup
  - On/Off Light Endpoint
  - LED GPIO-Steuerung (GPIO 8)
  - Attribute Update Callbacks
  - Event Handling
  - Commissioning Support
- [x] main/app_priv.h - Header-Datei
- [x] main/app_reset.h - Reset-Funktionen Header

### 3. Dokumentation erstellt
- [x] README.md - Ausführliche Installationsanleitung
- [x] QUICKSTART.md - Schnellstart-Guide
- [x] STRUCTURE.md - Detaillierte Projektarchitektur
- [x] .gitignore - Git-Konfiguration

### 4. Build-Automatisierung
- [x] build.sh - Automatisches Build-Skript
- [x] flash.sh - Automatisches Flash-Skript
- [x] Skripte ausführbar gemacht (chmod +x)

---

## 📋 Projekt-Details

### Hardware
- **Microcontroller**: ESP32-D0WD-V3 (Dual Core Xtensa, 240 MHz)
- **LED GPIO**: Automatisch konfiguriert durch ESP-Matter SDK
- **Flash Größe**: 4 MB
- **RAM**: 520 KB
- **MAC-Adresse**: d4:8c:49:e4:66:cc

### Software Stack
- **Framework**: ESP-IDF v5.1.2
- **Matter SDK**: ESP-Matter (main branch)
- **Protokoll**: Matter over WiFi
- **Commissioning**: Bluetooth LE (BLE)
- **Device Type**: On/Off Light (0x0100)

### Funktionen
✓ Matter-Protokoll Unterstützung
✓ Apple Home Integration
✓ WiFi Verbindung
✓ BLE Commissioning
✓ LED On/Off Steuerung
✓ Siri-Sprachsteuerung
✓ Home App Steuerung

### Wichtige Konfigurationen

**LED GPIO**:
```cpp
#define LED_GPIO GPIO_NUM_8
```

**Device Type**:
- Matter Device: On/Off Light
- Cluster: On/Off (0x0006)
- Attribute: OnOff (Boolean)

**Flash Partitionen**:
- nvs: 24 KB (NVS Storage)
- otadata: 8 KB (OTA Data)
- app0: 1.5 MB (Firmware Slot 1)
- app1: 1.5 MB (Firmware Slot 2)
- fctry: 24 KB (Factory Settings)

---

## 🔄 Nächste Schritte / Mögliche Erweiterungen

### Optionale Features (nicht implementiert)
- [ ] LED-Status in NVS speichern (Zustand nach Neustart wiederherstellen)
- [ ] Factory Reset Button (z.B. GPIO 9, 5 Sekunden gedrückt)
- [ ] Dimming-Funktion (PWM mit Level Control Cluster)
- [ ] Farbsteuerung (Color Control Cluster)
- [ ] Status-LED Blink-Pattern zur Identifikation
- [ ] Weitere Sensoren (Temperatur, Luftfeuchtigkeit)
- [ ] Deep Sleep Modus für Batteriebetrieb

### Empfohlene Anpassungen
- **LED GPIO prüfen**: Falls anderes ESP32-C3 Board, GPIO-Nummer anpassen
- **Device Name**: Vendor/Product Name in app_main.cpp anpassen
- **Logging Level**: Bei Bedarf in sdkconfig.defaults ändern

---

## 🚀 Build & Flash Anleitung

### Umgebung vorbereiten
```bash
# ESP-IDF aktivieren
source ~/esp/esp-idf/export.sh

# ESP-Matter aktivieren
source ~/esp/esp-matter/export.sh
```

### Projekt bauen
```bash
cd /Users/pitforster/Documents/Dev/ESP32_Apple_home/esp32_matter_light
./build.sh
```

### Flashen
```bash
# Auto-Detection
./flash.sh

# Manueller Port (macOS)
./flash.sh /dev/cu.usbserial-1410

# Manueller Port (Linux)
./flash.sh /dev/ttyUSB0
```

### Apple Home Pairing
1. Im seriellen Monitor: **Manual Pairing Code** notieren (z.B. 34970112332)
2. Home App öffnen → **+** → **Gerät hinzufügen**
3. **Weitere Optionen** → ESP32 auswählen
4. Pairing Code eingeben
5. Fertig!

---

## 📁 Erstellte Dateien

| Datei | Beschreibung | Status |
|-------|-------------|--------|
| `CMakeLists.txt` | Haupt-Build-Config | ✅ Erstellt |
| `partitions.csv` | Flash-Partitionen | ✅ Erstellt |
| `sdkconfig.defaults` | ESP32-C3 Config | ✅ Erstellt |
| `main/CMakeLists.txt` | Main Component Build | ✅ Erstellt |
| `main/app_main.cpp` | Hauptprogramm (206 Zeilen) | ✅ Erstellt |
| `main/app_priv.h` | Private Header | ✅ Erstellt |
| `main/app_reset.h` | Reset Header | ✅ Erstellt |
| `build.sh` | Build-Automatisierung | ✅ Erstellt |
| `flash.sh` | Flash-Automatisierung | ✅ Erstellt |
| `README.md` | Hauptdokumentation | ✅ Erstellt |
| `QUICKSTART.md` | Schnellstart-Guide | ✅ Erstellt |
| `STRUCTURE.md` | Architektur-Doku | ✅ Erstellt |
| `CLAUDE.md` | Claude Code Guidance | ✅ Erstellt |
| `.gitignore` | Git-Konfiguration | ✅ Erstellt |
| `PROJECT_STATUS.md` | Diese Status-Datei | ✅ Erstellt |

---

## 🐛 Bekannte Issues / Troubleshooting

### Issue: Gerät erscheint nicht in Home App
**Lösung**:
- 30-60 Sekunden warten
- Bluetooth am iPhone aktivieren
- ESP32 im gleichen WiFi-Netzwerk
- Logs prüfen: `idf.py monitor`

### Issue: LED funktioniert nicht
**Lösung**:
- GPIO-Nummer in app_main.cpp prüfen
- LED-Polarität testen (HIGH/LOW)
- Datenblatt des Boards konsultieren

### Issue: Build-Fehler
**Lösung**:
- ESP-IDF & ESP-Matter Umgebung aktivieren
- `rm -rf build` und neu bauen
- Versions-Kompatibilität prüfen (ESP-IDF v5.1.2)

---

## 📝 Notizen

- **Device Type**: On/Off Light ist der einfachste Matter Light Type
- **Commissioning**: Verwendet BLE für initiales Pairing, dann WiFi
- **Security**: Matter verwendet verschlüsselte Kommunikation
- **OTA Updates**: Durch Dual-Partition-Layout unterstützt
- **Apple Home**: Benötigt iOS 16.4+ für Matter-Unterstützung

---

## 🎯 Projektziel: ERREICHT ✅

**Ziel**: ESP32 App erstellen, die über Matter/WLAN mit Apple Home verbunden werden kann und die eingebaute LED steuert.

**Status**: ✅ Vollständig implementiert und deployed

**Ergebnis**:
- ✅ Funktionsfähige Matter-App für ESP32 (Dual Core)
- ✅ Erfolgreich mit Apple Home gepairt
- ✅ LED-Steuerung über Home App & Siri funktioniert
- ✅ On/Off Light (Device Type 0x0100)
- ✅ Vollständige Dokumentation
- ✅ Build-Automatisierung
- ✅ Erfolgreich auf Hardware getestet

---

## 📱 Deployment Status (2025-11-30)

### ✅ Erfolgreiches Apple Home Pairing
- **Gerät**: ESP32-D0WD-V3 (MAC: d4:8c:49:e4:66:cc)
- **Device Type**: On/Off Light
- **Pairing-Methode**: BLE Commissioning mit Manual Pairing Code
- **WiFi-Netzwerk**: "Tinkywinki"
- **Status**: Voll funktionsfähig in Apple Home integriert

### Commissioning Details
- **Manual Pairing Code**: 34970112332
- **QR Code**: MT:Y.K9042C00KA0648G00
- **Commissioning Window**: 300 Sekunden (5 Minuten)
- **Verbindung**: WiFi over Matter (nach BLE Commissioning)

### Testing
- ✅ Ein/Aus Steuerung via Home App funktioniert perfekt
- ✅ Siri Sprachsteuerung funktioniert
- ✅ LED schaltet korrekt (GPIO 2, direkte Steuerung)
- ✅ Matter-Protokoll funktioniert stabil

### Technische Implementierung

**LED-Steuerung:**
- **LED GPIO**: GPIO 2 (on-board LED auf ESP32 DevKit)
- **Steuerung**: Direkte GPIO-Kontrolle via `gpio_set_level()` in `app_driver.cpp`
- **Grund**: Bypassing ESP-Matter LED driver für vorhersagbare GPIO 2 Kontrolle
- **Logik**: GPIO HIGH (1) = LED AN, GPIO LOW (0) = LED AUS

**Factory Reset Button:**
- **Button GPIO**: GPIO 0 (BOOT Button auf ESP32 DevKit)
- **Funktion**: 3-5 Sekunden halten → Factory Reset → Alle Fabrics löschen
- **Vorteil**: Kein `idf.py erase-flash` mehr nötig zum Zurücksetzen
- **Auto-Commissioning**: Nach Reset öffnet sich automatisch neues Commissioning Window

**Dateien:**
- `main/app_driver.cpp`: Direkte GPIO-Steuerung implementiert
- `main/app_main.cpp`: GPIO 2 Blink-Test hinzugefügt
- `main/app_reset.cpp`: Factory Reset Button Implementation
- `main/app_reset.h`: Factory Reset Header

---

*Diese Datei wird von Claude Code automatisch gepflegt und dokumentiert den aktuellen Stand des Projekts.*
