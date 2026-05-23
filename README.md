# NullBeam
#  ESP32 Laser Tripwire with Telegram Alerts

* A compact ESP32‑based laser tripwire that latches the alarm and lets you reset it remotely via Telegram.*


![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino IDE](https://img.shields.io/badge/Arduino_IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)

##  Project Overview
The Laser Tripwire System is an effective security alarm built using an ESP32 microcontroller. It uses a continuous laser beam pointed directly at a Light Dependent Resistor (LDR). When the beam is broken, the light intensity drops and the analog reading spikes. 

**New Feature - Latching & Remote Control:** Once triggered, the alarm **latches** (the buzzer stays on indefinitely, even if the beam is restored). The ESP32 sends an instant ` ALERT!` message to your phone via Telegram. You can then use Telegram commands like `/status` to check on the system or `/reset` to remotely silence the buzzer and re-arm the tripwire!

##  How It Works
1. **The Beam:** A 3-pin laser module is hardwired to `3.3V` so it remains continuously on.
2. **The Sensor:** An LDR in a voltage divider sits at the other end. As long as the laser hits the LDR, the ESP32 analog reading stays very close to `0`.
3. **The Trigger:** When the beam is obstructed, the analog reading rises above the `threshold`. The system goes into a "Latched Alarm" state.
4. **The Alarm & Alert:** The ESP32 sends a `HIGH` signal to an active buzzer (which stays on) and sends an HTTP request to the Telegram Bot API alerting you of the intruder.
5. **Remote Reset:** Every second, the ESP32 checks Telegram for incoming messages. If it receives a `/reset` command from you, it disarms the buzzer and resets the alarm state, waiting for the next trip.

##  Components List
* **ESP32 DevKit V1 (30-pin)**
* **3-pin Laser Module**
* **Light Dependent Resistor (LDR) / Photoresistor**
* **10kΩ Resistor**
* **Active Buzzer**
* **Breadboard & Jumper Wires**

##  Pin Connections

| Component | Component Pin / Side | ESP32 Pin / Connection |
| :--- | :--- | :--- |
| **Laser Module** | S (Signal) / VCC | `3.3V` (Always ON) |
| | GND | `GND` |
| **LDR (Voltage Divider)** | LDR Leg 1 | `3.3V` |
| | LDR Leg 2 | `GPIO34` (Analog IN) **AND** 10kΩ Resistor |
| **10kΩ Resistor** | Resistor Leg 1 | Connected to LDR Leg 2 |
| | Resistor Leg 2 | `GND` |
| **Active Buzzer** | Positive (`+` or Long Leg)| `GPIO21` |
| | Negative (`-` or Short Leg)| `GND` |

##  Required Libraries
Install these via the Arduino IDE Library Manager:
* `UniversalTelegramBot` by Brian Lough
* `ArduinoJson` by Benoit Blanchon 

##  Telegram Bot Setup
1. Open Telegram and search for **@BotFather**.
2. Send `/newbot` and follow the instructions to name your bot.
3. Save the **HTTP API Token** BotFather gives you.
4. Search for **@myidbot** in Telegram and send `/getid` to get your **Chat ID**.
5. Start a conversation with your bot by searching its name and clicking **Start**.

##  Arduino IDE Code


Update the placeholders `YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, `YOUR_BOT_TOKEN`, and `YOUR_CHAT_ID` before uploading to your ESP32:

```cpp

const char* ssid = "REPLACE_WITH_YOUR_WIFI_SSID";
const char* password = "REPLACE_WITH_YOUR_WIFI_PASSWORD";

// Initialize Telegram BOT
#define BOT_TOKEN "REPLACE_WITH_YOUR_BOT_TOKEN" // Get from @BotFather
#define CHAT_ID "REPLACE_WITH_YOUR_CHAT_ID"     // Get from @myidbot


```

##  Testing & Calibration
1. **Align the Laser**: Ensure the laser is pointed directly at the center of the LDR.
2. **Open the Serial Monitor**: Set the baud rate to `115200`.
3. **Verify Baseline**: With the laser hitting the LDR, the analog reading should hover around `0`.
4. **Test the Tripwire**: Block the beam with your hand. The buzzer should sound, and you will receive a Telegram message.
5. **Test the Latch**: Remove your hand from the beam. The buzzer should **continue to sound**.
6. **Remote Reset**: Open Telegram and send `/reset` to the bot. The buzzer will turn off, and the system is armed again!

##  Troubleshooting Tips
* **No Telegram messages or bot not responding to commands?** 
  * Ensure your ESP32 is successfully connecting to Wi-Fi.
  * Your bot will only process commands from the exact `CHAT_ID` you provided in the code.
* **The buzzer won't turn off when starting!** 
  * If the laser isn't perfectly aligned *before* turning on the system, it will immediately trigger. Send `/reset` once you've aligned the beam.

##  Future Improvements
* **🕸️ Multiple Tripwires**: Add additional lasers and LDRs to different analog pins to create a web of tripwires around a room!

