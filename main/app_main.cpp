/*
 * ESP32-C3 Matter Light Control
 * Steuert die eingebaute LED über Apple Home via Matter
 */

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>

#include <app_priv.h>
#include <app_reset.h>

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

static const char *TAG = "ESP32_MATTER_LIGHT";

// GPIO Pin für die eingebaute LED des ESP32-C3
// Die meisten ESP32-C3 DevKits verwenden GPIO 8 für die eingebaute LED
#define LED_GPIO GPIO_NUM_8

// Endpoint und Cluster Handles
static uint16_t light_endpoint_id = 0;

/* Callback für Attribut-Updates */
static esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                          uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) {
        /* Wird vor dem Update aufgerufen */
        ESP_LOGI(TAG, "Attribute update: endpoint_id: %d, cluster_id: %ld, attribute_id: %ld",
                 endpoint_id, cluster_id, attribute_id);
    } else if (type == POST_UPDATE) {
        /* Wird nach dem Update aufgerufen - hier steuern wir die LED */

        // On/Off Cluster
        if (cluster_id == OnOff::Id && attribute_id == OnOff::Attributes::OnOff::Id) {
            bool led_state = val->val.b;
            ESP_LOGI(TAG, "LED wird %s", led_state ? "eingeschaltet" : "ausgeschaltet");

            // LED steuern (aktiv HIGH)
            gpio_set_level(LED_GPIO, led_state ? 1 : 0);
        }
    }

    return err;
}

/* Identifikations-Callback (z.B. wenn man das Gerät in Apple Home identifiziert) */
static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id,
                                        uint8_t effect_id, uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback: endpoint: %d, effect: %d", endpoint_id, effect_id);

    // Optional: LED blinken lassen zur Identifikation
    if (type == identification::callback_type_t::START) {
        ESP_LOGI(TAG, "Identifikation gestartet - LED blinkt");
        // Hier könnte man die LED blinken lassen
    } else if (type == identification::callback_type_t::STOP) {
        ESP_LOGI(TAG, "Identifikation gestoppt");
    }

    return ESP_OK;
}

/* Event Handler für Matter Events */
static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "IP-Adresse geändert");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning abgeschlossen - Gerät ist mit Apple Home verbunden!");
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning Fail-Safe Timer abgelaufen");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning Session gestartet");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning Session gestoppt");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning Fenster geöffnet");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning Fenster geschlossen");
        break;

    default:
        break;
    }
}

/* LED GPIO initialisieren */
static void app_driver_init()
{
    ESP_LOGI(TAG, "Initialisiere LED auf GPIO %d", LED_GPIO);

    // GPIO als Output konfigurieren
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_GPIO);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // LED initial ausschalten
    gpio_set_level(LED_GPIO, 0);
    ESP_LOGI(TAG, "LED initialisiert (ausgeschaltet)");
}

extern "C" void app_main()
{
    esp_err_t err = ESP_OK;

    ESP_LOGI(TAG, "ESP32-C3 Matter Light Starting...");

    /* NVS Flash initialisieren (für WiFi und Matter Konfiguration) */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* LED Driver initialisieren */
    app_driver_init();

    /* Matter Node erstellen */
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    if (!node) {
        ESP_LOGE(TAG, "Matter Node konnte nicht erstellt werden");
        return;
    }

    /* On/Off Light Endpoint erstellen */
    on_off_light::config_t light_config;
    light_config.on_off.on_off = false;  // Initial aus
    light_config.on_off.lighting.start_up_on_off = nullptr;  // Kein Startup-Verhalten

    endpoint_t *endpoint = on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, NULL);
    if (!endpoint) {
        ESP_LOGE(TAG, "Light Endpoint konnte nicht erstellt werden");
        return;
    }

    light_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Light Endpoint erstellt mit ID: %d", light_endpoint_id);

    /* Cluster-spezifische Konfiguration */
    cluster_t *cluster = cluster::get(endpoint, OnOff::Id);
    if (cluster) {
        cluster::on_off::feature::lighting::config_t lighting_config;
        lighting_config.lighting_level_control = 0;
        cluster::on_off::feature::lighting::add(cluster, &lighting_config);
    }

    /* Device Type: On/Off Light (0x0100) */
    ESP_LOGI(TAG, "Device Type: On/Off Light");

    /* Matter Stack starten */
    ESP_LOGI(TAG, "Starte Matter Stack...");
    err = esp_matter::start(app_event_cb);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Matter konnte nicht gestartet werden: %d", err);
        return;
    }

    /* Commissioning Fenster öffnen (für Pairing mit Apple Home) */
    // Mit Bluetooth LE
    ESP_LOGI(TAG, "Öffne Commissioning Fenster für Apple Home Pairing...");

    // QR Code und Manual Pairing Code werden in den Logs angezeigt
    ESP_LOGI(TAG, "===============================================");
    ESP_LOGI(TAG, "Bereit für Apple Home Pairing!");
    ESP_LOGI(TAG, "Öffne die Home App auf deinem iPhone/iPad");
    ESP_LOGI(TAG, "Tippe auf '+' -> 'Gerät hinzufügen'");
    ESP_LOGI(TAG, "Scanne den QR Code aus den Logs oder gib den Setup-Code manuell ein");
    ESP_LOGI(TAG, "===============================================");

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::init();
#endif
}
