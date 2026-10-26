# ⏰ Arduino SmartEnv Alarm Clock

![C++](https://img.shields.io/badge/Language-C++-00599C?style=for-the-badge&logo=c%2B%2B)
![Arduino](https://img.shields.io/badge/Platform-Arduino-00979C?style=for-the-badge&logo=arduino)
![PlatformIO](https://img.shields.io/badge/Build-PlatformIO-FF6200?style=for-the-badge&logo=platformio)
![License](https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge)

## 📖 Project Overview

The **SmartEnv Alarm Clock** is an embedded C++ project built for the Arduino platform that combines precise timekeeping with environmental monitoring. 

**The "Why" Principle: Architecture & Design**
Under the hood, this project avoids the pitfalls of standard "spaghetti code" often found in beginner Arduino projects by utilizing a **Finite State Machine (FSM)** for screen navigation and a centralized **Context Structure** (`struct context`). 
*   **State Machine:** By abstracting the UI into discrete states (e.g., `CLOCK_SCR`, `ALARM_SCR`, `SHOW_ENV_SCR`), the main loop remains clean, non-blocking, and highly responsive.
*   **Context Passing:** Instead of relying on global variables, hardware object pointers and system states are bundled into a `context` struct and passed by reference. This design pattern improves memory safety, encapsulation, and modularity.
*   **Non-volatile State:** The alarm parameters are continuously synced with the Arduino's EEPROM, ensuring that user configurations survive unexpected power losses.

## ✨ Features

*   **Real-Time Clock Synchronization:** Accurate time and date tracking via the DS1302 RTC module.
*   **Environmental Monitoring:** Live temperature and humidity readings using a DHT11 sensor.
*   **Non-Volatile Alarm Logic:** Alarms persist through power cycles via EEPROM storage.
*   **Visual & Audio Alerts:** Integrates an active buzzer and an RGB LED indicator for intuitive alarm feedback.
*   **Dynamic UI Navigation:** Context-aware button controls with debouncing for navigating settings, viewing dates, and monitoring the environment.
*   **Factory Reset Protocol:** Hard reset functionality to clear EEPROM corruption and restore default states.

## 🛠️ Hardware Requirements

Based on the firmware configurations, the following components are required:
*   **Microcontroller:** Arduino Uno / Nano (or any AVR-compatible board)
*   **Display:** 16x2 Character LCD with I2C Backpack
*   **RTC Module:** DS1302 Real-Time Clock
*   **Sensor:** DHT11 Temperature and Humidity Sensor
*   **Audio:** 5V Active Buzzer
*   **Indicators:** Common Cathode RGB LED
*   **Input:** 4x Momentary Pushbuttons (Active Low / Pull-up)
*   *Misc:* Breadboard, jumper wires, resistors (for LED/Buttons if hardware pull-ups aren't sufficient).

## 🔌 Pinout & Wiring Guide

| Component | Arduino Pin | Notes |
| :--- | :---: | :--- |
| **I2C LCD SDA** | `A4` | Data line for I2C communication (Address `0x27`). |
| **I2C LCD SCL** | `A5` | Clock line for I2C communication. |
| **RTC CLK** | `8` | DS1302 Clock Pin. |
| **RTC DAT** | `9` | DS1302 Data (I/O) Pin. |
| **RTC RST (CE)**| `10` | DS1302 Reset / Chip Enable Pin. |
| **DHT11 Data** | `7` | Temperature and humidity data line. |
| **Buzzer** | `12` | Alarm audio output. |
| **RGB LED (Red)** | `11` | Illuminates when alarm is active/ringing. |
| **RGB LED (Green)**| `6` | Visual feedback on alarm dismissal. |
| **Button 1** | `5` | Used for entering Alarm Set Mode & incrementing values. |
| **Button 2** | `4` | Used to view Date & shift edit modes. |
| **Button 3** | `3` | Used to view Environment & decrement values. |
| **Button 4** | `2` | Used to Toggle Alarm & confirm/save settings. |

*Note: Buttons utilize internal pull-up resistors (`INPUT_PULLUP`). Wire them to connect to `GND` when pressed.*

## 📦 Software Dependencies

The project relies on several external libraries. Ensure these are installed in your PlatformIO or Arduino IDE environment:

1.  **[I2C_LCD](https://github.com/johnrickman/LiquidCrystal_I2C)**: For driving the 16x2 LCD via I2C.
2.  **[Makuna RTC](https://github.com/Makuna/Rtc)**: Specifically utilizing `RtcDS1302.h` and `ThreeWire.h` for the DS1302 module.
3.  **[Adafruit DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library)**: For interfacing with the DHT11 (`DHT.h`).
4.  **EEPROM & Wire**: Standard Arduino built-in libraries.

## 🚀 Installation & Setup

1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/jabba000/smartenv-alarm.git
    cd smartenv-alarm
    ```
2.  **Wire the Hardware:** Follow the Pinout guide above to connect your modules to the Arduino.
3.  **Open Project:**
    *   *Using PlatformIO (Recommended):* Open the project folder in VS Code. PlatformIO will automatically read the `platformio.ini` file and download dependencies.
    *   *Using Arduino IDE:* Move the `.cpp` and `.h` files into a standard Arduino sketch folder structure, rename `main.cpp` to `main.ino` (and remove the `int main()` wrapper if using the standard IDE loop), and install the dependencies via the Library Manager.
4.  **Compile & Flash:**
    *   Click **Upload** in PlatformIO to compile and flash the firmware to your connected Arduino board.

## 🕹️ Usage Instructions

Once booted, the device will briefly display "Booting..." and load your saved alarm configurations from the EEPROM.

*   **Main Screen:** Displays the current time and alarm status (`ON`/`OFF`). The Red LED will light up if the alarm is enabled.
*   **Toggle Alarm:** Press **BTN4** on the main screen to turn the alarm ON or OFF.
*   **View Date:** Press **BTN2**. The date will show for 4 seconds. From here, press **BTN4** to enter **Time Setting Mode**.
*   **View Environment:** Press **BTN3**. Live temperature and humidity will display for 6 seconds.
*   **Set Alarm Time:** Press **BTN1** on the main screen. 
    *   Use **BTN1** to toggle between editing Hours and Minutes.
    *   Use **BTN2** to increase the value and **BTN3** to decrease it.
    *   Press **BTN4** to save and exit.
*   **Set Current Time/Date:** (From the Date View -> Press BTN4).
    *   Use the same BTN1/2/3 navigation to adjust the RTC parameters. Press BTN4 to commit changes to the RTC module.
*   **When Alarm Rings:** The buzzer will sound, the red LED will flash, and "WAKE UP!" will appear. 
    *   Press **BTN3** to Snooze (adds 1 minute to the alarm).
    *   Press **BTN4** to Dismiss (flashes the green LED and stops the alarm).
*   **Factory Reset:** From the main screen, press and hold **BTN1 + BTN4** simultaneously for 1.5 seconds. This wipes the EEPROM and resets the clock to `01/01/2026 00:00:00`.
