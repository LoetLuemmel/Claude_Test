#!/bin/bash

# ESP32-C3 Matter Light Flash Script
# Dieses Skript flasht den ESP32 und öffnet den Monitor

set -e

# Farben für Output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "======================================"
echo "ESP32-C3 Matter Light Flash Script"
echo "======================================"

# Port Parameter oder Auto-Detection
PORT=$1

if [ -z "$PORT" ]; then
    echo "Suche nach ESP32..."

    # macOS
    if [[ "$OSTYPE" == "darwin"* ]]; then
        PORT=$(ls /dev/cu.usbserial-* 2>/dev/null | head -n 1)
        if [ -z "$PORT" ]; then
            PORT=$(ls /dev/cu.SLAB_USBtoUART 2>/dev/null | head -n 1)
        fi
    # Linux
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        PORT=$(ls /dev/ttyUSB* 2>/dev/null | head -n 1)
    fi

    if [ -z "$PORT" ]; then
        echo -e "${RED}Fehler: Kein ESP32 gefunden${NC}"
        echo "Bitte Port manuell angeben:"
        echo "  ./flash.sh /dev/cu.usbserial-XXXX"
        exit 1
    fi
fi

echo -e "${GREEN}✓ ESP32 gefunden auf: $PORT${NC}"

# Flashen
echo ""
echo "Flashe ESP32..."
idf.py -p $PORT flash

echo ""
echo -e "${GREEN}Flash erfolgreich!${NC}"
echo ""
echo "Starte Monitor (Beenden mit Ctrl+])..."
sleep 1

# Monitor starten
idf.py -p $PORT monitor
