# Projektstruktur

```
esp32_matter_light/
│
├── CMakeLists.txt              # Haupt-Build-Konfiguration
├── partitions.csv              # Flash-Partitionstabelle
├── sdkconfig.defaults          # ESP-IDF Standard-Konfiguration
│
├── main/
│   ├── CMakeLists.txt          # Build-Konfiguration für main Component
│   ├── app_main.cpp            # Hauptprogramm mit Matter-Logik
│   ├── app_priv.h              # Private Header-Datei
│   └── app_reset.h             # Reset-Funktionen Header
│
├── build.sh                    # Build-Automatisierungsskript
├── flash.sh                    # Flash-Automatisierungsskript
│
├── README.md                   # Ausführliche Dokumentation
├── QUICKSTART.md               # Schnellstart-Anleitung
├── STRUCTURE.md                # Diese Datei - Projektstruktur
└── .gitignore                  # Git Ignore-Regeln
```

## Datei-Beschreibungen

### Konfigurationsdateien

#### `CMakeLists.txt`
Haupt-CMake-Konfiguration für das ESP-IDF-Projekt. Definiert:
- Projekt-Name
- ESP-Matter Integration
- Target-Device (ESP32-C3)
- Matter Device Type (On/Off Light)

#### `sdkconfig.defaults`
Standard-Konfiguration für ESP-IDF. Enthält:
- ESP32-C3 spezifische Einstellungen
- Bluetooth/BLE Konfiguration (für Commissioning)
- WiFi-Parameter
- Partitionstabellen-Referenz
- Logging-Level
- Compiler-Optimierungen

#### `partitions.csv`
Flash-Partitionstabelle, definiert:
- **nvs**: Non-Volatile Storage für WiFi/Matter-Daten
- **otadata**: OTA Update Daten
- **app0/app1**: Dual-OTA Partitionen für Firmware-Updates
- **fctry**: Factory-Partition für Werkseinstellungen

### Quellcode

#### `main/app_main.cpp`
Hauptprogramm mit folgenden Funktionen:

**Callbacks:**
- `app_attribute_update_cb()`: Behandelt Attribut-Updates (z.B. LED on/off)
- `app_identification_cb()`: Behandelt Identifikations-Anfragen
- `app_event_cb()`: Matter Event Handler

**Initialisierung:**
- `app_driver_init()`: GPIO-Initialisierung für LED
- `app_main()`: Hauptfunktion
  - NVS initialisieren
  - Matter Node erstellen
  - On/Off Light Endpoint hinzufügen
  - Matter Stack starten
  - Commissioning aktivieren

**Wichtige Konstanten:**
```cpp
#define LED_GPIO GPIO_NUM_8  // LED GPIO-Pin
```

#### `main/app_priv.h` & `main/app_reset.h`
Header-Dateien für zukünftige Erweiterungen (z.B. Factory Reset Button)

### Build-Skripte

#### `build.sh`
Automatisiertes Build-Skript:
- Prüft ESP-IDF und ESP-Matter Umgebung
- Setzt Target auf ESP32-C3
- Führt Build-Prozess aus
- Gibt Status-Meldungen aus

Verwendung:
```bash
./build.sh
```

#### `flash.sh`
Automatisiertes Flash-Skript:
- Auto-Detection des seriellen Ports
- Flasht das Firmware-Image
- Startet automatisch den seriellen Monitor

Verwendung:
```bash
./flash.sh                      # Auto-Detection
./flash.sh /dev/cu.usbserial-0  # Manueller Port
```

## Matter Device-Architektur

```
┌─────────────────────────────────────┐
│         Matter Stack                │
│  (Thread/WiFi + Matter Protocol)    │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│         Matter Node                 │
│  - Device Information               │
│  - Network Commissioning            │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│      On/Off Light Endpoint          │
│  ┌────────────────────────────┐     │
│  │   On/Off Cluster           │     │
│  │   - OnOff Attribute        │◄────┼──── Apple Home
│  └────────────────────────────┘     │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│         LED Driver                  │
│  - GPIO Kontrolle (GPIO 8)          │
│  - Hardware Steuerung               │
└─────────────────────────────────────┘
```

## Matter Clusters

### On/Off Cluster (0x0006)
- **Attribute**:
  - `OnOff` (0x0000): Boolean - LED Status
- **Commands**:
  - `Off`: LED ausschalten
  - `On`: LED einschalten
  - `Toggle`: LED umschalten

### Basic Information Cluster (0x0028)
- Vendor Name
- Product Name
- Hardware/Software Version
- Serial Number

### Network Commissioning Cluster (0x0031)
- WiFi SSID/Credentials Management
- Network Status

## Commissioning-Prozess

```
┌─────────┐                  ┌──────────┐                 ┌────────────┐
│ iPhone  │                  │  ESP32   │                 │ WiFi AP    │
│  Home   │                  │  Matter  │                 │            │
└────┬────┘                  └─────┬────┘                 └─────┬──────┘
     │                             │                            │
     │  1. BLE Scan                │                            │
     ├────────────────────────────►│                            │
     │                             │                            │
     │  2. BLE Connection          │                            │
     ├────────────────────────────►│                            │
     │                             │                            │
     │  3. Passcode Verification   │                            │
     ├────────────────────────────►│                            │
     │                             │                            │
     │  4. WiFi Credentials        │                            │
     ├────────────────────────────►│                            │
     │                             │                            │
     │                             │  5. WiFi Connect           │
     │                             ├───────────────────────────►│
     │                             │                            │
     │  6. Matter Operational Cert │                            │
     ├────────────────────────────►│                            │
     │                             │                            │
     │  7. Done - Control via WiFi │                            │
     ├◄───────────────────────────►│◄───────────────────────────┤
     │                             │                            │
```

## Erweiterungsmöglichkeiten

### Dimming hinzufügen (Level Control)
```cpp
// In app_main.cpp ändern von:
endpoint_t *endpoint = on_off_light::create(...)

// Zu:
endpoint_t *endpoint = dimmable_light::create(...)

// PWM für LED-Dimming implementieren
```

### Factory Reset Button
```cpp
// GPIO Button definieren
#define RESET_BUTTON GPIO_NUM_9

// Button-Handler Task erstellen
// Bei 5 Sekunden gedrückt: Factory Reset
```

### Temperatur-Sensor hinzufügen
```cpp
// Neuen Endpoint für Temperature Sensor
temperature_sensor::config_t temp_config;
endpoint_t *temp_endpoint = temperature_sensor::create(node, &temp_config, ...);
```

### OTA Updates
```cpp
// ESP-Matter OTA ist bereits integriert
// Matter Controller (Apple Home) kann OTA Updates durchführen
```

## Build-Ausgaben

Nach `idf.py build`:

```
build/
├── bootloader/
│   └── bootloader.bin          # Bootloader
├── partition_table/
│   └── partition-table.bin     # Partitionstabelle
├── esp32_matter_light.bin      # Haupt-Firmware
└── flash_project_args          # Flash-Parameter
```

## Wichtige Logs

### Beim Start
```
I ESP32_MATTER_LIGHT: ESP32-C3 Matter Light Starting...
I ESP32_MATTER_LIGHT: Initialisiere LED auf GPIO 8
I ESP32_MATTER_LIGHT: Light Endpoint erstellt mit ID: 1
I ESP32_MATTER_LIGHT: Starte Matter Stack...
```

### Commissioning
```
I CHIP: [SVR] QR Code URL: https://...
I CHIP: [SVR] Manual pairing code: 34970112332
I ESP32_MATTER_LIGHT: Commissioning abgeschlossen
```

### LED-Steuerung
```
I ESP32_MATTER_LIGHT: Attribute update: endpoint_id: 1, cluster_id: 6, attribute_id: 0
I ESP32_MATTER_LIGHT: LED wird eingeschaltet
```

## Ressourcen-Nutzung

Ungefähre Werte:

- **Flash**: ~2.5 MB (von 4 MB)
- **RAM**: ~200 KB (von 400 KB)
- **CPU**: <5% idle, ~15% bei Netzwerk-Aktivität
- **Stromverbrauch**:
  - Idle: ~40 mA
  - Active: ~80-120 mA
  - Deep Sleep: ~10 µA (falls implementiert)
