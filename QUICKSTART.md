# Quick Start Guide

## Schnellinstallation

### 1. Voraussetzungen installieren (macOS)

```bash
# Homebrew installieren (falls nicht vorhanden)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Benötigte Tools
brew install cmake ninja dfu-util python3
```

### 2. ESP-IDF & ESP-Matter Setup

```bash
# Verzeichnis erstellen
mkdir -p ~/esp && cd ~/esp

# ESP-IDF klonen
git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32c3
cd ..

# ESP-Matter klonen
git clone --depth 1 -b main https://github.com/espressif/esp-matter.git
cd esp-matter
git submodule update --init --depth 1
cd ./connectedhomeip/connectedhomeip
./scripts/checkout_submodules.py --platform esp32 linux --shallow
cd ../..
./install.sh
cd ..
```

### 3. Umgebung aktivieren

```bash
# ESP-IDF
source ~/esp/esp-idf/export.sh

# ESP-Matter
source ~/esp/esp-matter/export.sh
```

### 4. Projekt bauen und flashen

```bash
# Zum Projektverzeichnis
cd /Users/pitforster/Documents/Dev/Claude_Test/esp32_matter_light

# Target setzen
idf.py set-target esp32c3

# Bauen
idf.py build

# Flashen (Port anpassen!)
# macOS: /dev/cu.usbserial-*
# Linux: /dev/ttyUSB0
idf.py -p /dev/cu.usbserial-1410 flash monitor
```

### 5. Mit Apple Home verbinden

1. Im seriellen Monitor erscheint ein **Manual Pairing Code** (z.B. `34970112332`)
2. Öffne die **Home App** auf iPhone/iPad
3. Tippe auf **+ → Gerät hinzufügen → Weitere Optionen**
4. Wähle dein Gerät aus der Liste
5. Gib den Pairing Code ein
6. Fertig - LED lässt sich nun steuern!

## Testen

Nach erfolgreicher Verbindung:

```
"Hey Siri, schalte das Licht ein"
"Hey Siri, schalte das Licht aus"
```

## Tipps

- **Serieller Monitor beenden**: `Ctrl+]`
- **Gerät neu starten**: Reset-Button am ESP32 drücken
- **Kompletter Neuflash**: `idf.py erase-flash` dann erneut `flash`
- **Logs ansehen**: `idf.py monitor`

## Häufige Ports

### macOS
```bash
ls /dev/cu.*
# Typisch: /dev/cu.usbserial-* oder /dev/cu.SLAB_USBtoUART
```

### Linux
```bash
ls /dev/ttyUSB*
# Typisch: /dev/ttyUSB0
```

### Windows
```
COM3, COM4, etc.
# Im Device Manager prüfen
```

## LED GPIO Übersicht

Ich nahm hier einfach ein Joy-it.de board.

| Board | LED GPIO |
|-------|----------|
| ESP32dev | GPIO 2 |
| Custom Boards | Datenblatt prüfen |

Falls LED nicht funktioniert, GPIO in `main/app_main.cpp` anpassen:

```cpp
#define LED_GPIO GPIO_NUM_X  // X durch korrekte GPIO-Nummer ersetzen
```
