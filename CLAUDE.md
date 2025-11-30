# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an ESP32 Matter Light implementation that enables control of an onboard LED through Apple Home via the Matter protocol over WiFi. The device acts as an On/Off Light (Matter Device Type 0x0100) using BLE for commissioning.

**Hardware**: ESP32-D0WD-V3 (Dual Core Xtensa @ 240MHz)
**Status**: ✅ Successfully deployed and paired with Apple Home (2025-11-30)

## Build System & Environment

### Required Environment Setup

This project requires **ESP-IDF v5.1.2** and **ESP-Matter SDK** to be installed and activated:

```bash
# Activate ESP-IDF
source ~/esp/esp-idf/export.sh

# Activate ESP-Matter
source ~/esp/esp-matter/export.sh
```

**CRITICAL**: Both environments must be active before any build commands. The build will fail without these.

### Build Commands

```bash
# Set target (first time only)
idf.py set-target esp32

# Build project
idf.py build

# Or use automation script
./build.sh

# Clean build
rm -rf build && idf.py build

# Complete erase (factory reset)
idf.py erase-flash
```

### Flash & Monitor

```bash
# Flash and monitor (auto-detect port on macOS/Linux)
./flash.sh

# Manual flash with specific port
idf.py -p /dev/cu.usbserial-XXXX flash monitor

# Monitor only
idf.py -p /dev/cu.usbserial-XXXX monitor

# Exit monitor: Ctrl+]
```

Common serial ports:
- macOS: `/dev/cu.usbserial-*` or `/dev/cu.SLAB_USBtoUART`
- Linux: `/dev/ttyUSB0`

## Architecture

### Matter Device Hierarchy

```
Matter Node (root)
└── On/Off Light Endpoint (endpoint_id: 1)
    ├── Basic Information Cluster (device metadata)
    ├── On/Off Cluster (0x0006)
    │   └── OnOff Attribute (Boolean)
    └── Identify Cluster (for pairing identification)
```

### Code Flow

1. **Initialization** (`app_main()`):
   - Initialize NVS flash (stores WiFi credentials and Matter data)
   - Initialize LED GPIO driver
   - Create Matter node with callbacks
   - Create On/Off Light endpoint
   - Start Matter stack
   - Open commissioning window (BLE)

2. **Attribute Updates** (`app_attribute_update_cb()`):
   - PRE_UPDATE: Called before attribute changes
   - POST_UPDATE: Called after attribute changes - **this is where LED control happens**
   - Checks for OnOff Cluster (0x0006) and OnOff Attribute
   - Directly controls GPIO based on boolean value

3. **Event Handling** (`app_event_cb()`):
   - Monitors Matter stack events (IP changes, commissioning status, etc.)
   - Logs important state transitions

### Key Constants

- **LED GPIO**: GPIO 2 (on-board LED on ESP32 DevKit)
  - Directly controlled via `gpio_set_level()` in `app_driver.cpp`
  - **Important**: Bypasses ESP-Matter LED driver for direct control
  - GPIO HIGH (1) = LED ON
  - GPIO LOW (0) = LED OFF

- **Matter Clusters**:
  - OnOff Cluster ID: `0x0006`
  - OnOff Attribute ID: `0x0000`

- **Device Type**: On/Off Light (0x0100)
  - Simple on/off control without dimming or color
  - Changed from Extended Color Light to reduce complexity

### LED Control Implementation

The LED is controlled directly via GPIO without using the ESP-Matter LED driver abstraction:

**Location**: `main/app_driver.cpp:35-48`

```cpp
static esp_err_t app_driver_light_set_power(led_driver_handle_t handle, esp_matter_attr_val_t *val)
{
    // Direct GPIO control for GPIO 2
    // Matter ON (1) → GPIO HIGH (1) → LED ON
    // Matter OFF (0) → GPIO LOW (0) → LED OFF
    uint32_t gpio_level = val->val.b ? 1 : 0;
    gpio_set_level(GPIO_NUM_2, gpio_level);
    return ESP_OK;
}
```

**Why Direct Control?**
- ESP-Matter's LED driver may use different GPIO pins depending on the board
- Direct control ensures GPIO 2 is used (on-board LED on most ESP32 DevKit boards)
- Simpler and more predictable for this single-LED use case

## Configuration Files

- **`sdkconfig.defaults`**: Generic config supporting multiple ESP32 variants (Bluetooth, WiFi, partitions)
- **`partitions.csv`**: Flash layout with dual-OTA support
  - nvs: 24KB (credentials storage)
  - app0/app1: 1.5MB each (dual OTA partitions)
  - fctry: 24KB (factory settings)

## Apple Home Commissioning

After flashing, the device automatically opens a commissioning window. The serial monitor will display:

```
I CHIP: [SVR] Manual pairing code: 34970112332
I CHIP: [SVR] QR Code: MT:Y.K9042C00KA0648G00
```

**Recommended: Use the 11-digit Manual Pairing Code** (`34970112332`)

The commissioning flow:
1. Home App → **+** → **Add Accessory** → **More Options**
2. Select the ESP32 device
3. Enter manual code: `34970112332`
4. BLE discovery and connection
5. Passcode verification
6. WiFi credential provisioning
7. Matter operational certificate exchange
8. Device control via WiFi

**Note on QR Code**:
- The QR code may not work reliably with non-certified Matter development boards
- Apple Home's QR scanner has strict requirements for certified devices
- The manual pairing code is the recommended method for development/testing

## Common Modifications

### Changing LED GPIO

Edit `main/app_driver.cpp:23`:
```cpp
#define LED_GPIO GPIO_NUM_X  // Replace X with your GPIO number (currently GPIO_NUM_2)
```

Then update the `app_driver_light_set_power()` function if your LED has different polarity (active-LOW vs active-HIGH).

### Adding Device Metadata

Before `node::create()` in `app_main()`:
```cpp
node_config.root_node.basic_information.vendor_name = "YourBrand";
node_config.root_node.basic_information.product_name = "Your Product";
```

### Upgrading to Dimmable Light

Change from `on_off_light::create()` to `dimmable_light::create()` and implement PWM control in the attribute callback for Level Control Cluster (0x0008).

## Important Notes

- **No state persistence**: LED state is not saved to NVS. After reboot, the device starts with LED off. Matter controller (Apple Home) will re-sync state.
- **Direct GPIO Control**: LED is controlled directly via `gpio_set_level()`, bypassing ESP-Matter LED driver
  - GPIO 2 on ESP32 DevKit: GPIO HIGH (1) = LED ON, GPIO LOW (0) = LED OFF
- **BLE + WiFi**: Device uses BLE only for initial commissioning, then operates over WiFi
- **OTA Ready**: Dual-partition layout supports OTA updates via Matter controller

## File Reference

- **`main/app_main.cpp`**: Core application logic with Matter node setup
- **`main/app_driver.cpp`**: Direct GPIO LED control implementation
- **`main/app_reset.cpp`**: Factory reset button functionality (GPIO 0)
- **`main/app_reset.h`**: Factory reset function declarations
- **`CMakeLists.txt`**: Project build configuration with Matter integration
- **`sdkconfig.defaults`**: ESP32 hardware configuration
- **`partitions.csv`**: Flash memory layout (dual-OTA support)
- **`build.sh`**: Automated build with environment checks
- **`flash.sh`**: Automated flash with port auto-detection
- **`PROJECT_STATUS.md`**: Complete project status and implementation checklist

## Resources

- ESP-IDF v5.1.2 Documentation: https://docs.espressif.com/projects/esp-idf/
- ESP-Matter SDK: https://github.com/espressif/esp-matter
- Matter Specification: https://csa-iot.org/all-solutions/matter/

---

## Deployment Information (2025-11-30)

### Successfully Deployed to Apple Home

**Hardware:**
- ESP32-D0WD-V3 (Dual Core, 240MHz)
- MAC: d4:8c:49:e4:66:cc
- Port: /dev/cu.usbserial-110

**Pairing Details:**
- Manual Pairing Code: `34970112332`
- QR Code: `MT:Y.K9042C00KA0648G00`
- Commissioning: BLE → WiFi
- Network: "Tinkywinki"

**Status:** ✅ Fully operational in Apple Home
- Device appears as On/Off Light
- Controllable via Home App and Siri
- **LED control working perfectly** (GPIO 2, direct control)
- Matter protocol working stably

**Implementation Notes:**
- LED is controlled directly via `gpio_set_level(GPIO_NUM_2, val)` in `app_driver.cpp`
- Bypasses ESP-Matter LED driver for predictable GPIO 2 control
- GPIO HIGH = LED ON, GPIO LOW = LED OFF

### Factory Reset

**Method 1: Hardware Button (Recommended)**

Press and hold the **BOOT button (GPIO 0)** on the ESP32 DevKit for **3-5 seconds**:

1. Hold BOOT button → Serial monitor shows: `"Factory reset triggered. Release the button to start factory reset."`
2. Release button → Factory reset starts
3. Device restarts automatically
4. New commissioning window opens
5. Ready for re-pairing with QR code or manual code

**Method 2: Command Line (Alternative)**

To completely reset via command line:
```bash
idf.py erase-flash
idf.py -p /dev/cu.usbserial-110 flash
```

**When to use Factory Reset:**
- After removing device from Apple Home (if commissioning window doesn't open automatically)
- When switching between different Matter controllers
- To clear all pairing data and start fresh
