
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Pin Definitions

// Pin Definitions
const int zeroCrossPin = 4;  // GPIO4 (D2 on NodeMCU)
const int dimmerPin = 5;     // GPIO5 (D1 on NodeMCU)
const int LED_PIN = 13;      // LED 1
const int LED2 = 12;         
const int LED3 = 14 ;        
const int FAN_PIN = 15;
//const int LED1_BTN = 1;
//const int LED2_BTN = 1;
//const int LED3_BTN = 00;



// Fan control relay (Choose GPIO13 or any free GPIO pin)

// Variables


volatile bool zeroCrossDetected = false;  // Flag for zero-cross event
int dimmerDelay = 1500;  
//bool led1State = LOW;                     // State of LED 1
//bool led2State = LOW; 
//bool led3State = LOW;                     // State of LED 3
      
#define EEPROM_SIZE 10
#define BRIGHTNESS_ADDR 0
#define LED1_STATE_ADDR 1
#define LED2_STATE_ADDR 2
#define LED3_STATE_ADDR 3
#define FAN_STATE_ADDR 4


// Initial dimmer delay for brightness 1023
ESP8266WebServer server(80);
// WiFi credentials
const char* ssid = "SmartHome";
const char* password = "12345678";

// Interrupt Service Routine (ISR) for zero-cross detection
void ICACHE_RAM_ATTR zeroCrossISR() {
  zeroCrossDetected = true;
}
//

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
      <h2>Dimmer Control</h2>
      <p>
        Brightness: 
        <input type='range' min='0' max='3500' value='{{brightness}}' id='brightness' oninput='updateBrightness(this.value)' />
        <span id='brightnessValue'>{{brightness}}</span>
      </p>
      <script>
        document.getElementById('brightness').addEventListener('input', function(e) {
          document.getElementById('brightnessValue').textContent = e.target.value;
        });

        function updateBrightness(value) {
          fetch('/setBrightness?value=' + value)
            .then(response => response.text())
            .then(data => location.reload())  // Reload to update feedback
            .catch(error => console.error('Error:', error));
        }
      </script>
      <h2>LED and Fan Controls</h2>
      
      <p>
        LED 1: {{led1State}}<br>
        LED 2: {{led2State}}<br>
        LED 3: {{led3State}}<br>
        Fan: {{fanState}}<br>
      </p>
      <p>
        <a href="/H">Turn On LED 1</a><br>
        <a href="/L">Turn Off LED 1</a><br>aa
        <a href="/P">Turn On LED 2</a><br>
        <a href="/N">Turn Off LED 2</a><br>
        <a href="/A">Turn On LED 3</a><br>
        <a href="/B">Turn Off LED 3</a><br>
        <a href="/fanOn">Turn On Fan</a><br>
        <a href="/fanOff">Turn Off Fan</a><br>
      </p>
    </body>
    </html>
  )rawliteral";

  // Replace placeholders with current state
  html.replace("{{brightness}}", String(map(dimmerDelay, 500, 2000, 0, 3500)));
  html.replace("{{led1State}}", digitalRead(LED_PIN) == HIGH ? "ON" : "OFF");
  html.replace("{{led2State}}", digitalRead(LED2) == HIGH ? "ON" : "OFF");
  html.replace("{{led3State}}", digitalRead(LED3) == HIGH ? "ON" : "OFF");
  html.replace("{{fanState}}", digitalRead(FAN_PIN) == HIGH ? "ON" : "OFF");

  server.send(200, "text/html", html);
}

void handleSetBrightness() {
 if (server.hasArg("value")) {
    int brightness = server.arg("value").toInt();

    // Map the brightness value to dimmer delay
    dimmerDelay = map(brightness, 0, 1023, 500, 2000);
    
    // Save the new brightness value to EEPROM
    saveToEEPROM(BRIGHTNESS_ADDR, brightness);
    server.send(200, "text/plain", "Brightness set to " + String(brightness));
  } else {
    server.send(400, "text/plain", "Missing value");
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  // Load saved brightness or set default value
  int savedBrightness = readFromEEPROM(BRIGHTNESS_ADDR);
  savedBrightness = (savedBrightness == -1) ? 2500 : savedBrightness;  // Set default if EEPROM value is invalid
  saveToEEPROM(BRIGHTNESS_ADDR, savedBrightness);

  dimmerDelay = map(savedBrightness, 0, 3500, 500, 2000);

  // Set device states based on EEPROM values
  digitalWrite(LED_PIN, readFromEEPROM(LED1_STATE_ADDR));
  digitalWrite(LED2, readFromEEPROM(LED2_STATE_ADDR));
  digitalWrite(LED3, readFromEEPROM(LED3_STATE_ADDR));
  digitalWrite(FAN_PIN, readFromEEPROM(FAN_STATE_ADDR));

  // Set up Zero-Cross pin as input with pullup
  pinMode(zeroCrossPin, INPUT_PULLUP);

  // Set up dimmer control pin and LEDs as outputs
  pinMode(dimmerPin, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  // Attach interrupt to zero-cross pin (falling edge)
  attachInterrupt(digitalPinToInterrupt(zeroCrossPin), zeroCrossISR, FALLING);

  // Configure ESP8266 as an Access Point
  WiFi.softAP(ssid, password);  // Set up Access Point
  Serial.println("Access Point created!");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());  // Print AP IP address

  server.begin();
  Serial.println("Web server started!");

  // Initialize dimmer delay for brightness 1023
  dimmerDelay = map(1023, 0, 1023, 500, 2000);

  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setBrightness", HTTP_GET, handleSetBrightness);

  // LED control routes
  server.on("/H", [savedBrightness]() { // Capture savedBrightness in the lambda
    digitalWrite(LED_PIN, HIGH);
    saveToEEPROM(LED1_STATE_ADDR, HIGH);
    handleRoot();
  });

  server.on("/L", [savedBrightness]() { // Capture savedBrightness in the lambda
    digitalWrite(LED_PIN, LOW);
    saveToEEPROM(LED1_STATE_ADDR, LOW);
    handleRoot();
  });

  server.on("/P", [savedBrightness]() { // Capture savedBrightness in the lambda
    digitalWrite(LED2, HIGH);
    saveToEEPROM(LED2_STATE_ADDR, HIGH);
    handleRoot();
  });

  server.on("/N", [savedBrightness]() { // Capture savedBrightness in the lambda
    digitalWrite(LED2, LOW);
    saveToEEPROM(LED2_STATE_ADDR, LOW);
    handleRoot();
  });

  server.on("/A", [savedBrightness]() { // Capture savedBrightness in the lambda
    digitalWrite(LED3, HIGH);
    saveToEEPROM(LED3_STATE_ADDR, HIGH);
    handleRoot();
  });

  server.on("/B", [savedBrightness]() { // Capture savedBrightness in the lambda
    digitalWrite(LED3, LOW);
    saveToEEPROM(LED3_STATE_ADDR, LOW);
    handleRoot();
  });

  server.on("/fanOn", [savedBrightness]() { // Capture savedBrightness in the lambda
    digitalWrite(FAN_PIN, HIGH);
    saveToEEPROM(FAN_STATE_ADDR, HIGH);
    handleRoot();
  });

  server.on("/fanOff", [savedBrightness]() { // Capture savedBrightness in the lambda
    digitalWrite(FAN_PIN, LOW);
    saveToEEPROM(FAN_STATE_ADDR, LOW);
    handleRoot();
  });

  // API routes for checking states
  server.on("/LED1State", []() {
    int state = digitalRead(LED_PIN);
    String jsonResponse = "{\"LED1State\": \"" + String(state == HIGH ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", jsonResponse);
  });

  server.on("/LED2State", []() {
    int state = digitalRead(LED2);
    String jsonResponse = "{\"LED2State\": \"" + String(state == HIGH ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", jsonResponse);
  });

  server.on("/LED3State", []() {
    int state = digitalRead(LED3);
    String jsonResponse = "{\"LED3State\": \"" + String(state == HIGH ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", jsonResponse);
  });

  server.on("/fanState", []() {
    int state = digitalRead(FAN_PIN);
    String jsonResponse = "{\"fanState\": \"" + String(state == HIGH ? "ON" : "OFF") + "\"}";
    server.send(200, "application/json", jsonResponse);
  });

  server.on("/fanSpeed", [savedBrightness]() { // Capture savedBrightness in the lambda
    String jsonResponse = "{\"fanSpeed\": " + String(savedBrightness) + "}";
    server.send(200, "application/json", jsonResponse);
  });
}


// Main loop
void loop() {
  server.handleClient();
      // Handle incoming HTTP requests

  // Adjust brightness based on zero-cross detection and delay
  if (zeroCrossDetected) {
    zeroCrossDetected = false;  // Reset the flag

    // Generate a delay for controlling dimmer brightness
    delayMicroseconds(dimmerDelay);  // Adjust dimming time

    // Trigger the dimmer pin (control the AC power)
    digitalWrite(dimmerPin, HIGH);  // Turn on the dimmer (trigger TRIAC)
    delayMicroseconds(10);          // Hold for a brief time to trigger TRIAC
    digitalWrite(dimmerPin, LOW);   // Turn off the dimmer
  }
}
