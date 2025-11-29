#!/bin/bash

# ESP32-C3 Matter Light Build Script
# Dieses Skript automatisiert den Build-Prozess

set -e  # Bei Fehler abbrechen

echo "======================================"
echo "ESP32-C3 Matter Light Build Script"
echo "======================================"

# Farben für Output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Funktion: ESP-IDF Umgebung prüfen
check_idf() {
    if [ -z "$IDF_PATH" ]; then
        echo -e "${RED}Fehler: ESP-IDF Umgebung nicht aktiviert${NC}"
        echo "Bitte ausführen: source ~/esp/esp-idf/export.sh"
        exit 1
    fi
    echo -e "${GREEN}✓ ESP-IDF gefunden: $IDF_PATH${NC}"
}

# Funktion: ESP-Matter Umgebung prüfen
check_matter() {
    if [ -z "$ESP_MATTER_PATH" ]; then
        echo -e "${RED}Fehler: ESP-Matter Umgebung nicht aktiviert${NC}"
        echo "Bitte ausführen: source ~/esp/esp-matter/export.sh"
        exit 1
    fi
    echo -e "${GREEN}✓ ESP-Matter gefunden: $ESP_MATTER_PATH${NC}"
}

# Umgebung prüfen
echo "Prüfe Umgebung..."
check_idf
check_matter

# Target setzen
echo ""
echo "Setze Target auf ESP32-C3..."
idf.py set-target esp32c3

# Build
echo ""
echo "Starte Build-Prozess..."
idf.py build

echo ""
echo -e "${GREEN}======================================"
echo "Build erfolgreich abgeschlossen!"
echo "======================================${NC}"
echo ""
echo "Zum Flashen verwende:"
echo "  ./flash.sh [PORT]"
echo ""
echo "Oder manuell:"
echo "  idf.py -p /dev/cu.usbserial-XXXX flash monitor"
