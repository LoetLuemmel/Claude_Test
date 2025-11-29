# CMakeLists.txt für ESP32-C3 Matter Light
cmake_minimum_required(VERSION 3.5)

# ESP-Matter Pfad
set(ESP_MATTER_PATH $ENV{ESP_MATTER_PATH})

# Setze den ESP32-C3 als Target
set(IDF_TARGET esp32c3)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_matter_light)

# Matter Device Type: On/Off Light
idf_build_set_property(MATTER_DEVICE_TYPE "lighting" APPEND)
