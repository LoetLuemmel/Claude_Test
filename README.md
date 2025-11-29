# ESP32-C3 Matter Light für Apple Home

Dieses Projekt ermöglicht es, die eingebaute LED eines ESP32-C3 über Apple Home via Matter-Protokoll zu steuern.

## Funktionen

- Matter-Unterstützung (Project CHIP)
- Kompatibel mit Apple Home
- Steuert die eingebaute LED des ESP32-C3
- On/Off Light Device Type
- Bluetooth LE Commissioning

## Hardware-Anforderungen

- ESP32-C3 Development Board (z.B. ESP32-C3-DevKitM-1)
- USB-Kabel
- iPhone/iPad mit iOS 16.4+ und Home App

## Software-Anforderungen

### 1. ESP-IDF Installation

```bash
# ESP-IDF v5.1 oder neuer installieren
mkdir -p ~/esp
cd ~/esp
git clone -b v5.1.2 --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32c3
. ./export.sh
```

### 2. ESP-Matter SDK Installation

```bash
# ESP-Matter installieren
cd ~/esp
git clone --depth 1 -b main https://github.com/espressif/esp-matter.git
cd esp-matter
git submodule update --init --depth 1
cd ./connectedhomeip/connectedhomeip
./scripts/checkout_submodules.py --platform esp32 linux --shallow
cd ../..
./install.sh
```

### 3. Umgebungsvariablen setzen

Füge diese Zeilen zu deiner `~/.bashrc` oder `~/.zshrc` hinzu:

```bash
# ESP-IDF
export IDF_PATH=~/esp/esp-idf
alias get_idf='. $IDF_PATH/export.sh'

# ESP-Matter
export ESP_MATTER_PATH=~/esp/esp-matter
alias get_matter='. $ESP_MATTER_PATH/export.sh'
```

Nach dem Hinzufügen, aktiviere die Umgebung:

```bash
source ~/.bashrc  # oder ~/.zshrc
get_idf
get_matter
```

## Projekt Bauen

### 1. Zum Projektverzeichnis wechseln

```bash
cd /Users/pitforster/Documents/Dev/Claude_Test/esp32_matter_light
```

### 2. Konfiguration setzen

```bash
# ESP-IDF und ESP-Matter Umgebung aktivieren
get_idf
get_matter

# Konfiguration (optional, da sdkconfig.defaults vorhanden)
idf.py set-target esp32c3
```

### 3. Projekt bauen

```bash
idf.py build
```

### 4. Auf ESP32-C3 flashen

```bash
# USB Port anpassen (z.B. /dev/ttyUSB0 auf Linux, /dev/cu.usbserial-* auf macOS)
idf.py -p /dev/cu.usbserial-1410 flash monitor
```

Oder nur flashen:

```bash
idf.py -p /dev/cu.usbserial-1410 flash
```

## Gerät mit Apple Home verbinden

### 1. Commissioning-Informationen erhalten

Nach dem Flash und Start des ESP32-C3, öffne den seriellen Monitor:

```bash
idf.py -p /dev/cu.usbserial-1410 monitor
```

Im Log wirst du folgende Informationen finden:

- **QR Code**: Ein ASCII QR-Code zum Scannen
- **Manual Pairing Code**: Ein 11-stelliger Code (z.B. 34970112332)
- **Discriminator**: 4-stellige Nummer
- **Passcode**: 8-stellige Nummer

Beispiel Log-Ausgabe:
```
I (12345) CHIP: [SVR] QR Code URL: https://project-chip.github.io/connectedhomeip/qrcode.html?data=MT:...
I (12346) CHIP: [SVR] Manual pairing code: 34970112332
```

### 2. In Apple Home hinzufügen

1. Öffne die **Home App** auf deinem iPhone/iPad
2. Tippe auf **+** (oben rechts)
3. Wähle **Gerät hinzufügen** oder **Accessoire hinzufügen**
4. Wähle **Weitere Optionen...**
5. Dein ESP32-C3 sollte als "Matter Accessory" erscheinen
6. Tippe darauf und wähle eine der folgenden Optionen:

   **Option A: QR-Code scannen**
   - Falls ein QR-Code im Log angezeigt wird, scanne diesen

   **Option B: Code manuell eingeben**
   - Gib den 11-stelligen Manual Pairing Code ein (z.B. `34970112332`)

7. Folge den Anweisungen auf dem Bildschirm
8. Das Gerät wird als "Light" hinzugefügt

### 3. LED steuern

Nach erfolgreicher Verbindung kannst du:

- Die LED über die Home App ein-/ausschalten
- Siri verwenden: "Hey Siri, schalte das Licht ein"
- Automationen erstellen
- Das Gerät in Szenen einbinden

## GPIO Konfiguration

Die Standard-LED-GPIO ist **GPIO 8** (üblich bei ESP32-C3 DevKits).

Falls dein Board eine andere LED-GPIO verwendet, ändere in `main/app_main.cpp`:

```cpp
#define LED_GPIO GPIO_NUM_8  // Ändere zu deiner LED GPIO
```

Häufige LED-GPIOs bei ESP32-C3 Boards:
- **GPIO 8**: ESP32-C3-DevKitM-1
- **GPIO 2**: Manche Custom Boards
- **GPIO 18** oder **GPIO 19**: Alternative Boards

## Troubleshooting

### Commissioning schlägt fehl

1. Stelle sicher, dass iPhone/iPad und ESP32-C3 im **gleichen WiFi-Netzwerk** sind
2. Überprüfe, dass Bluetooth auf dem iPhone aktiviert ist
3. Lösche das Gerät aus der Home App und versuche es erneut
4. Factory Reset des ESP32: Flash komplett löschen mit `idf.py erase-flash`

### Gerät erscheint nicht in der Home App

1. Warte 30-60 Sekunden nach dem Start
2. Überprüfe die Logs: `idf.py monitor`
3. Stelle sicher, dass das Commissioning-Fenster offen ist
4. Starte den ESP32 neu

### Build-Fehler

1. Stelle sicher, dass ESP-IDF und ESP-Matter korrekt installiert sind
2. Aktiviere die Umgebung: `get_idf && get_matter`
3. Lösche den Build-Ordner: `rm -rf build` und baue erneut

### LED funktioniert nicht

1. Überprüfe die GPIO-Nummer in `app_main.cpp`
2. Teste die LED manuell:
   ```bash
   # Im seriellen Monitor oder separaten Terminal
   gpio_set_level(8, 1);  # LED an
   gpio_set_level(8, 0);  # LED aus
   ```

## Device Informationen

- **Device Type**: On/Off Light (0x0100)
- **Vendor ID**: Standard Matter Vendor
- **Product ID**: Konfigurierbar
- **Commissioning**: BLE (Bluetooth Low Energy)
- **Network**: WiFi

## Erweiterte Konfiguration

### Device Information ändern

Bearbeite `main/app_main.cpp` und füge vor `node::create()` hinzu:

```cpp
node_config.root_node.basic_information.vendor_name = "MeineMarke";
node_config.root_node.basic_information.product_name = "Mein Smart Light";
node_config.root_node.basic_information.hardware_version = 1;
node_config.root_node.basic_information.software_version = 1;
```

### Factory Reset aktivieren

Füge einen Reset-Button hinzu, z.B. GPIO 9:

```cpp
// In app_main.cpp
#define RESET_BUTTON GPIO_NUM_9

// Button Init
gpio_config_t btn_conf = {};
btn_conf.pin_bit_mask = (1ULL << RESET_BUTTON);
btn_conf.mode = GPIO_MODE_INPUT;
btn_conf.pull_up_en = GPIO_PULLUP_ENABLE;
gpio_config(&btn_conf);

// In einer Task: Button für 5 Sekunden gedrückt = Factory Reset
```

## Lizenz

Dieses Projekt verwendet ESP-IDF und ESP-Matter, die unter Apache 2.0 Lizenz stehen.

## Ressourcen

- [ESP-IDF Dokumentation](https://docs.espressif.com/projects/esp-idf/)
- [ESP-Matter Dokumentation](https://docs.espressif.com/projects/esp-matter/)
- [Matter Spezifikation](https://csa-iot.org/all-solutions/matter/)
- [Apple Home Developer](https://developer.apple.com/home/)

## Support

Bei Problemen:
1. Überprüfe die Logs mit `idf.py monitor`
2. Konsultiere die [ESP-Matter Examples](https://github.com/espressif/esp-matter/tree/main/examples)
3. Erstelle ein Issue im Projekt-Repository
