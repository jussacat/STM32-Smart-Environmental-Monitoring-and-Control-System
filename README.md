# Smart Environmental Monitoring & Control System

A real-time environmental monitoring and control system based on **STM32F103, FreeRTOS, DHT22, SSD1306 OLED, DS3231 RTC, and ESP32**.

The system measures temperature and humidity, displays real-time information on an OLED, controls a relay based on temperature thresholds, and provides remote monitoring and control through an ESP32 Wi-Fi web dashboard.

---

## Features

- Temperature and humidity monitoring using DHT22
- Real-time clock using DS3231
- Real-time data display on SSD1306 OLED
- Multitasking using FreeRTOS
- Automatic relay control based on temperature
- UART communication between STM32 and ESP32
- Wi-Fi connectivity through ESP32
- Web dashboard for remote monitoring
- Auto / Manual control modes
- UART debug console
- Temperature and humidity history
- Fault detection and error handling

---

## System Architecture

```text
                         ┌─────────────────┐
                         │      DHT22      │
                         │ Temperature     │
                         │ Humidity        │
                         └────────┬────────┘
                                  │
                                  │ GPIO
                                  ▼
┌───────────────┐         ┌─────────────────┐
│    DS3231     │◄──I2C──►│                 │
│      RTC      │         │    STM32F103    │
└───────────────┘         │                 │
                          │    FreeRTOS     │
┌───────────────┐         │                 │
│    SSD1306    │◄──I2C──►│  Sensor Task    │
│     OLED      │         │  Display Task   │
└───────────────┘         │  Control Task   │
                          │  UART Task      │
┌───────────────┐         │                 │
│     Relay     │◄─GPIO──│                 │
└───────────────┘         └────────┬────────┘
                                   │
                                  UART
                                   │
                                   ▼
                          ┌─────────────────┐
                          │      ESP32      │
                          │                 │
                          │ Wi-Fi           │
                          │ Web Server      │
                          └────────┬────────┘
                                   │
                                  Wi-Fi
                                   │
                                   ▼
                          ┌─────────────────┐
                          │ Web Dashboard   │
                          │ PC / Smartphone │
                          └─────────────────┘
