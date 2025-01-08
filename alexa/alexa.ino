#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Espalexa.h>

// Pin Definitions
const int LED1 = 13; 
const int LED2 = 12; 
const int LED3 = 14; 
const int LED4 = 16;

const int LED1Switch = 4;  
const int LED2Switch = 5;  
const int LED3Switch = 2;  
const int LED4Switch = 0;  

#define EEPROM_SIZE 10
#define LED1_STATE_ADDR 0
#define LED2_STATE_ADDR 1
#define LED3_STATE_ADDR 2
#define LED4_STATE_ADDR 3

ESP8266WebServer server(80);
Espalexa espalexa;

// Wi-Fi credentials (replace with your home network details)
const char* ssid = "TechInfoSync"; // Your router's SSID
const char* password = "Admin@1234"; // Your router's password

void controlLED1(uint8_t brightness) {
  digitalWrite(LED1, brightness == 0 ? LOW : HIGH);
}
void controlLED2(uint8_t brightness) {
  digitalWrite(LED2, brightness == 0 ? LOW : HIGH);
}
void controlLED3(uint8_t brightness) {
  digitalWrite(LED3, brightness == 0 ? LOW : HIGH);
}
void controlLED4(uint8_t brightness) {
  digitalWrite(LED4, brightness == 0 ? LOW : HIGH);
}

void saveToEEPROM(int addr, int value) {
  EEPROM.write(addr, value);
  EEPROM.commit();
}

int readFromEEPROM(int addr) {
  return EEPROM.read(addr);
}

// Handle the root webpage
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <body>
      <h1>Smart Home Control</h1>
      <h2>LED Controls</h2>
      <p>
        LED 1: {{led1State}}<br>
        LED 2: {{led2State}}<br>
        LED 3: {{led3State}}<br>
        LED 4: {{led4State}}<br>
      </p>
      <p>
        <a href="/H">Turn On LED 1</a><br>
        <a href="/L">Turn Off LED 1</a><br>
        <a href="/P">Turn On LED 2</a><br>
        <a href="/N">Turn Off LED 2</a><br>
        <a href="/A">Turn On LED 3</a><br>
        <a href="/B">Turn Off LED 3</a><br>
        <a href="/C">Turn On LED 4</a><br>
        <a href="/D">Turn Off LED 4</a><br>
      </p>
    </body>
    </html>
  )rawliteral";

  html.replace("{{led1State}}", digitalRead(LED1) == HIGH ? "ON" : "OFF");
  html.replace("{{led2State}}", digitalRead(LED2) == HIGH ? "ON" : "OFF");
  html.replace("{{led3State}}", digitalRead(LED3) == HIGH ? "ON" : "OFF");
  html.replace("{{led4State}}", digitalRead(LED4) == HIGH ? "ON" : "OFF");

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  // Configure pins
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(LED1Switch, INPUT_PULLUP);
  pinMode(LED2Switch, INPUT_PULLUP);
  pinMode(LED3Switch, INPUT_PULLUP);
  pinMode(LED4Switch, INPUT_PULLUP);

  digitalWrite(LED1, readFromEEPROM(LED1_STATE_ADDR));
  digitalWrite(LED2, readFromEEPROM(LED2_STATE_ADDR));
  digitalWrite(LED3, readFromEEPROM(LED3_STATE_ADDR));
  digitalWrite(LED4, readFromEEPROM(LED4_STATE_ADDR));

  espalexa.addDevice("First Load", controlLED1);
  espalexa.addDevice("Second Load", controlLED2);
  espalexa.addDevice("Third Load", controlLED3);
  espalexa.addDevice("Fourth Load", controlLED4);
  espalexa.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print the IP address

  server.begin();
  Serial.println("Web server started!");

  server.on("/", HTTP_GET, handleRoot);

  server.on("/H", []() {
    digitalWrite(LED1, HIGH);
    saveToEEPROM(LED1_STATE_ADDR, HIGH);
    handleRoot();
  });
  server.on("/L", []() {
    digitalWrite(LED1, LOW);
    saveToEEPROM(LED1_STATE_ADDR, LOW);
    handleRoot();
  });
  server.on("/P", []() {
    digitalWrite(LED2, HIGH);
    saveToEEPROM(LED2_STATE_ADDR, HIGH);
    handleRoot();
  });
  server.on("/N", []() {
    digitalWrite(LED2, LOW);
    saveToEEPROM(LED2_STATE_ADDR, LOW);
    handleRoot();
  });
  server.on("/A", []() {
    digitalWrite(LED3, HIGH);
    saveToEEPROM(LED3_STATE_ADDR, HIGH);
    handleRoot();
  });
  server.on("/B", []() {
    digitalWrite(LED3, LOW);
    saveToEEPROM(LED3_STATE_ADDR, LOW);
    handleRoot();
  });
  server.on("/C", []() {
    digitalWrite(LED4, HIGH);
    saveToEEPROM(LED4_STATE_ADDR, HIGH);
    handleRoot();
  });
  server.on("/D", []() {
    digitalWrite(LED4, LOW);
    saveToEEPROM(LED4_STATE_ADDR, LOW);
    handleRoot();
  });
}

void loop() {
  server.handleClient();
    espalexa.loop();


  static bool prevLED1SwitchState = HIGH;
  static bool prevLED2SwitchState = HIGH;
  static bool prevLED3SwitchState = HIGH;
  static bool prevLED4SwitchState = HIGH;

  // Read current switch states
  bool currentLED1SwitchState = digitalRead(LED1Switch);
  bool currentLED2SwitchState = digitalRead(LED2Switch);
  bool currentLED3SwitchState = digitalRead(LED3Switch);
  bool currentLED4SwitchState = digitalRead(LED4Switch);

  // Check for LED1 toggle
  if (currentLED1SwitchState == LOW && prevLED1SwitchState == HIGH) {
    digitalWrite(LED1, !digitalRead(LED1)); // Toggle LED1 state
    saveToEEPROM(LED1_STATE_ADDR, digitalRead(LED1)); // Save state to EEPROM
    delay(200); // Debounce delay
  }
  prevLED1SwitchState = currentLED1SwitchState;

  // Check for LED2 toggle
  if (currentLED2SwitchState == LOW && prevLED2SwitchState == HIGH) {
    digitalWrite(LED2, !digitalRead(LED2)); // Toggle LED2 state
    saveToEEPROM(LED2_STATE_ADDR, digitalRead(LED2)); // Save state to EEPROM
    delay(200); // Debounce delay
  }
  prevLED2SwitchState = currentLED2SwitchState;

  // Check for LED3 toggle
  if (currentLED3SwitchState == LOW && prevLED3SwitchState == HIGH) {
    digitalWrite(LED3, !digitalRead(LED3)); // Toggle LED3 state
    saveToEEPROM(LED3_STATE_ADDR, digitalRead(LED3)); // Save state to EEPROM
    delay(200); // Debounce delay
  }
  prevLED3SwitchState = currentLED3SwitchState;

  // Check for LED4 toggle
  if (currentLED4SwitchState == LOW && prevLED4SwitchState == HIGH) {
    digitalWrite(LED4, !digitalRead(LED4)); // Toggle LED4 state
    saveToEEPROM(LED4_STATE_ADDR, digitalRead(LED4)); // Save state to EEPROM
    delay(200); // Debounce delay
  }
  prevLED4SwitchState = currentLED4SwitchState;
}
