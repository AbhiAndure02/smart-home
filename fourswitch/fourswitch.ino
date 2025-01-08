#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


// Pin Definitions
const int LED1 = 13; // LED 1
const int LED2 = 12; // LED 2
const int LED3 = 14; // LED 3
const int LED4 = 16; // LED 

const int LED1Switch = 4;  // GPIO4 (D2 on NodeMCU)
const int LED2Switch = 5;  // GPIO5 (D1 on NodeMCU)
const int LED3Switch = 2; // GPIO16 (D0 on NodeMCU)
const int LED4Switch = 0;  // GPIO0 (D3 on NodeMCU)

#define EEPROM_SIZE 10
#define LED1_STATE_ADDR 0
#define LED2_STATE_ADDR 1
#define LED3_STATE_ADDR 2
#define LED4_STATE_ADDR 3






ESP8266WebServer server(80);




// WiFi credentials
const char* ssid = "SmartHome";
const char* password = "12345678";




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

  // Replace placeholders with current state
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

  // Restore states from EEPROM
  digitalWrite(LED1, readFromEEPROM(LED1_STATE_ADDR));
  digitalWrite(LED2, readFromEEPROM(LED2_STATE_ADDR));
  digitalWrite(LED3, readFromEEPROM(LED3_STATE_ADDR));
  digitalWrite(LED4, readFromEEPROM(LED4_STATE_ADDR));

  

  // Set up WiFi and web server
  WiFi.softAP(ssid, password);
  Serial.println("Access Point created!");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.begin();
  Serial.println("Web server started!");

  // Define web server routes
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


   server.on("/LED1State", [] () {
    int state = digitalRead(LED1);
    String jsonResponse = "{\"LED1State\": \"" + String(state == HIGH ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", jsonResponse);
  });

  server.on("/LED2State", [] () {
    int state = digitalRead(LED2);
    String jsonResponse = "{\"LED2State\": \"" + String(state == HIGH ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", jsonResponse);
  });

  server.on("/LED3State", [] () {
    int state = digitalRead(LED3);
    String jsonResponse = "{\"LED3State\": \"" + String(state == HIGH ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", jsonResponse);
  });

  server.on("/LED4State", [] () {
    int state = digitalRead(LED4);
    String jsonResponse = "{\"LED1State\": \"" + String(state == HIGH ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", jsonResponse);
  });
}

void loop() {
  server.handleClient();

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
