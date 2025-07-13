#include <WiFiNINA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

const char* ssid = "YourSSID";
const char* wifiPassword = "YourWiFiPassword";

WiFiServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // updates every minute

const int relayPin = 8;

#define MAX_EVENTS 20
#define PASSWORD_ADDR 500
#define PASSWORD_MAX_LEN 16

struct BellEvent {
  uint8_t hour;
  uint8_t minute;
  uint8_t rings;
};

BellEvent schedule[MAX_EVENTS];
int numEvents = 0;
String adminPassword = "admin123"; // default password

unsigned long lastNtpSync = 0;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  EEPROM.get(0, numEvents);
  if (numEvents > 0 && numEvents <= MAX_EVENTS) {
    EEPROM.get(sizeof(int), schedule);
  } else {
    numEvents = 0;
  }
  loadPasswordFromEEPROM();

  WiFi.begin(ssid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());

  timeClient.begin();
  syncTimeWithNTP();
  lastNtpSync = millis();

  server.begin();
}

void loop() {
  timeClient.update();

  if (millis() - lastNtpSync > 3600000) { // every hour
    syncTimeWithNTP();
    lastNtpSync = millis();
  }

  checkAndRingBell();
  handleWebRequests();
}

void syncTimeWithNTP() {
  if (timeClient.update()) {
    Serial.println("NTP synced: " + String(timeClient.getFormattedTime()));
  }
}

void checkAndRingBell() {
  int currentHour = hourNow();
  int currentMinute = minuteNow();
  int currentSecond = secondNow();

  for (int i = 0; i < numEvents; i++) {
    if (currentHour == schedule[i].hour && currentMinute == schedule[i].minute && currentSecond == 0) {
      ringBell(schedule[i].rings);
      delay(60000); // prevent re-triggering same minute
    }
  }
}

void ringBell(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(relayPin, HIGH);
    delay(1000);
    digitalWrite(relayPin, LOW);
    delay(1000);
  }
}

void handleWebRequests() {
  WiFiClient client = server.available();
  if (!client) return;

  String req = client.readStringUntil('\r');
  client.flush();

  if (req.indexOf("/add?") >= 0 && req.indexOf("pwd=" + adminPassword) >= 0) {
    int h = getParam(req, "hour");
    int m = getParam(req, "minute");
    int r = getParam(req, "rings");
    if (numEvents < MAX_EVENTS && h >= 0 && m >= 0 && r > 0) {
      schedule[numEvents++] = { (uint8_t)h, (uint8_t)m, (uint8_t)r };
      saveSchedule();
    }
  } else if (req.indexOf("/changepwd?") >= 0) {
    String oldpwd = getStringParam(req, "oldpwd");
    String newpwd = getStringParam(req, "newpwd");
    if (oldpwd == adminPassword && newpwd.length() <= PASSWORD_MAX_LEN) {
      adminPassword = newpwd;
      savePasswordToEEPROM();
    }
  }

  // Respond with HTML
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println(generateHTMLPage());
}

String generateHTMLPage() {
  String page = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<style>body{font-family:sans-serif;max-width:500px;margin:auto;background:#f5f5f5;padding:20px;}"
    ".card{background:#fff;border-radius:10px;padding:15px;margin-bottom:15px;box-shadow:0 2px 6px rgba(0,0,0,0.1);}"
    ".event{display:flex;justify-content:space-between;margin:8px 0;}"
    ".form-row{display:flex;justify-content:space-between;margin-top:10px;}"
    "input{width:30%;padding:5px;border:1px solid #ccc;border-radius:5px;}"
    "input[type='password'], input.full{width:100%;margin-top:10px;}"
    "button{background:#4CAF50;color:white;border:none;border-radius:5px;padding:8px 12px;cursor:pointer;margin-top:10px;}"
    "</style><title>School Bell Scheduler</title></head><body>"
    "<h1>🔔 School Bell Scheduler</h1>";

  page += "<div class='card'>";
  page += "<div><strong>Current Time:</strong> " + timeClient.getFormattedTime() + "</div>";
  page += "<div><strong>Next Bell:</strong> --:--</div></div>";

  page += "<div class='card'>";
  for (int i = 0; i < numEvents; i++) {
    page += "<div class='event'>" + String(schedule[i].hour) + ":" + (schedule[i].minute < 10 ? "0" : "") + String(schedule[i].minute) +
            " - Rings: " + String(schedule[i].rings) + "</div>";
  }
  page += "</div>";

  page += "<div class='card'>"
    "<form action='/add' method='get'>"
    "<div class='form-row'>"
    "<input type='number' name='hour' min='0' max='23' placeholder='Hour' required>"
    "<input type='number' name='minute' min='0' max='59' placeholder='Minute' required>"
    "<input type='number' name='rings' min='1' max='10' placeholder='Rings' required>"
    "</div>"
    "<input type='password' name='pwd' placeholder='Password' class='full' required>"
    "<button type='submit'>➕ Add Bell Time</button></form>";

  page += "<form action='/changepwd' method='get'>"
    "<input type='password' name='oldpwd' placeholder='Old Password' class='full' required>"
    "<input type='password' name='newpwd' placeholder='New Password' class='full' required>"
    "<button type='submit'>🔑 Change Password</button></form>";

  page += "</div></body></html>";
  return page;
}

int getParam(String req, String key) {
  int idx = req.indexOf(key + "=");
  if (idx < 0) return -1;
  int end = req.indexOf('&', idx);
  String val = (end < 0) ? req.substring(idx + key.length() + 1) : req.substring(idx + key.length() + 1, end);
  return val.toInt();
}

String getStringParam(String req, String key) {
  int idx = req.indexOf(key + "=");
  if (idx < 0) return "";
  int end = req.indexOf('&', idx);
  return (end < 0) ? req.substring(idx + key.length() + 1) : req.substring(idx + key.length() + 1, end);
}

void saveSchedule() {
  EEPROM.put(0, numEvents);
  EEPROM.put(sizeof(int), schedule);
  EEPROM.commit();
}

void loadPasswordFromEEPROM() {
  char pwd[PASSWORD_MAX_LEN + 1];
  for (int i = 0; i < PASSWORD_MAX_LEN; i++) {
    pwd[i] = EEPROM.read(PASSWORD_ADDR + i);
    if (pwd[i] == '\0') break;
  }
  pwd[PASSWORD_MAX_LEN] = '\0';
  if (strlen(pwd) > 0) adminPassword = String(pwd);
}

void savePasswordToEEPROM() {
  for (unsigned int i = 0; i < adminPassword.length() && i < PASSWORD_MAX_LEN; i++) {
    EEPROM.write(PASSWORD_ADDR + i, adminPassword[i]);
  }
  EEPROM.write(PASSWORD_ADDR + adminPassword.length(), '\0');
  EEPROM.commit();
}

// helper functions to get time parts from NTP
int hourNow()   { return (timeClient.getEpochTime()  % 86400L) / 3600; }
int minuteNow() { return (timeClient.getEpochTime() % 3600) / 60; }
int secondNow() { return timeClient.getEpochTime() % 60; }
