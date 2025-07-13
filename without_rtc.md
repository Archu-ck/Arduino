# 🔔 WiFi School Bell without RTC

WiFi-enabled automatic school bell using Arduino + NTP.  
No RTC; gets time from internet.

---

## ✅ Features
- Add bell times + number of rings from phone (password protected)
- Change password
- Auto-sync time from NTP
- Control DC motor (as bell) via relay

---

## ⚙ Materials
| Item | Purpose |
|--|--|
| Arduino Uno WiFi / MKR WiFi / Nano 33 IoT | Main controller |
| Relay module | Switch DC motor |
| DC motor | Acts as bell |
| Jumper wires | Connections |

---

## 🛠 Wiring

| Relay | Connects To |
|--|--|
| VCC | Arduino 5V |
| GND | Arduino GND |
| IN | Arduino D8 |

**Motor:**
- Power supply + → relay COM
- relay NO → motor +
- motor – → power supply GND

---

## 📲 How to use
- Upload sketch (set your WiFi SSID/password)
- Open Serial Monitor → get Arduino IP
- Open browser on same WiFi → `http://<IP_ADDRESS>`
- Use web interface to add bell times, change password
- Motor rings as bell at scheduled times

---

## 🔐 Default password
`admin123`

---

## ✏ Customize
- Change ring speed/duration in `ringBell()`.

---

## 🧰 License
Free for educational use.
