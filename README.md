# 🔔 WiFi‑Enabled School Timer Bell using Arduino

An automatic school bell system based on Arduino Uno WiFi (or similar), 
that rings a bell (DC motor) at scheduled times.  
The schedule can be edited easily from a web interface on your phone.

---

## ✅ **Features**
- Add / edit bell times and number of rings from phone (with password)
- Auto-sync time via NTP every hour
- Stores schedule and password in EEPROM (survives reboot)
- Web interface served by Arduino
- Simple DC motor / buzzer used as bell (controlled via relay)

---

## ⚙ **Materials Required**
| Item | Recommended | Purpose |
|--|--|--|
| Arduino Uno WiFi (or MKR WiFi / Nano 33 IoT) | ✔️ | Main controller with WiFi |
| DS3231 RTC module | ✔️ | Accurate timekeeping |
| Relay module (5V) | ✔️ | Safely switch DC motor or bell |
| DC motor / buzzer | ✔️ | Acts as bell |
| Jumper wires | ✔️ | Connections |
| Breadboard (optional) | ✔️ | Testing |


---

## 🛠 **Wiring (step by step)**

### 📅 **RTC module (DS3231)**
Uses I2C (SDA/SCL):
- **SDA → Arduino A4**
- **SCL → Arduino A5**
- VCC → 5V
- GND → GND

> *RTC and other I2C devices share same SDA/SCL pins.*

---

### 🔧 **Relay module**
- Relay **IN pin → Arduino digital pin D8**
- VCC → 5V
- GND → GND

Relay **NO/COM terminals** connect to the DC motor’s power circuit.

---

### 🛎 **DC motor as bell**
- Connect external DC power supply (e.g., 5V or 12V depending on your motor)
- Connect **one wire of motor power** to relay **COM**
- Connect **NO** to positive of motor
- Other motor terminal → power supply ground

Relay acts as a switch: closes circuit → motor spins → bell rings.

---

## 📲 **How to use**
1. Upload the Arduino sketch (update your WiFi SSID & password in code).
2. Open **Serial Monitor** (9600 baud) → see the assigned IP address.
3. On your phone / PC (same WiFi): open browser → go to `http://<IP_ADDRESS>`.
4. Use the web interface to:
   - View schedule
   - Add new bell times (with password)
   - Change password
5. Bell rings automatically at scheduled times.
6. Time auto-syncs with NTP every hour (no need to set manually).

---

## 🔐 **Default password**
- `admin123`

You can change it anytime from the web interface.

---

## 🚀 **Built with**
- Arduino IDE
- WiFiNINA library
- RTClib
- EEPROM library
- NTPClient

---

## ✏ **Customize**
- Change ring duration / interval in `ringBell()` function.
- Adjust web page HTML for custom style or features.

---

