#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Pin Definitions
const int LED1 = 4; // LED 1
const int LED2 = 5; // LED 2
const int LED3 = 14; // LED 3
const int LED4 = 13; // LED 4

const int LED1Switch = 12;  // GPIO4 (D2 on NodeMCU)
const int LED2Switch = 15;  // GPIO5 (D1 on NodeMCU)
const int LED3Switch = 16;  // GPIO16 (D0 on NodeMCU)
const int LED4Switch = 3;   // GPIO0 (D3 on NodeMCU)

ESP8266WebServer server(80);

// Store the Wi-Fi credentials
String ssid = "";
String password = "";

// HTML template for the configuration page

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<body>
  <h1>Connect to Your Wi-Fi Router</h1>
  <form action="/connect" method="POST">
    SSID: <input type="text" name="ssid" required><br><br>
    Password: <input type="password" name="password" required><br><br>
    <input type="submit" value="Connect">
    
  </form>
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

// HTML page to show the IP address after successful connection
String successPage = R"rawliteral(
<!DOCTYPE html>
<html>
<body>
  <h1>Successfully connected to Wi-Fi!</h1>
  <p>Your router's IP address is: %s</p>
</body>
</html>
)rawliteral";

// Handle the root page where the user can input Wi-Fi credentials


// Handle the form submission to connect to the router
void handleConnect() {
  ssid = server.arg("ssid");
  password = server.arg("password");

  // Set the static IP address, gateway, and subnet mask
  IPAddress localIP(192, 168, 4, 1);     // Static IP you want to set
  IPAddress gateway(192, 168, 1, 102);       // Default gateway (your router's IP)
  IPAddress subnet(255, 255, 255, 0);      // Subnet mask

  // Set the static IP configuration
  WiFi.config(localIP, gateway, subnet);

  WiFi.begin(ssid.c_str(), password.c_str());

  // Wait for the connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    String ipAddress = WiFi.localIP().toString();
    successPage.replace("%s", ipAddress.c_str());
    server.send(200, "text/html", successPage);
  } else {
    server.send(200, "text/html", "<h1>Failed to connect. Please try again.</h1>");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Set up LED pins
  // Configure pins
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
 pinMode(LED3, OUTPUT);
 pinMode(LED4, OUTPUT);

  pinMode(LED1Switch, INPUT_PULLUP);
  pinMode(LED2Switch, INPUT_PULLUP);
  pinMode(LED3Switch, INPUT_PULLUP);
  pinMode(LED4Switch, INPUT_PULLUP);


  //initially all default off
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);


  // Set up Wi-Fi in Access Point mode
  WiFi.softAP("TechHome", "12345678");
  Serial.println("Access Point created!");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start the server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/connect", HTTP_POST, handleConnect);

  server.begin();
  Serial.println("Web server started!");

  server.on("/", HTTP_GET, handleRoot);

  server.on("/H", []() {
    digitalWrite(LED1, HIGH);
    handleRoot();
  });
  server.on("/L", []() {
    digitalWrite(LED1, LOW);
    handleRoot();
  });
  server.on("/P", []() {
    digitalWrite(LED2, HIGH);
    handleRoot();
  });
  server.on("/N", []() {
    digitalWrite(LED2, LOW);
    handleRoot();
  });
  server.on("/A", []() {
    digitalWrite(LED3, HIGH);
    handleRoot();
  });
  server.on("/B", []() {
    digitalWrite(LED3, LOW);
    handleRoot();
});
server.on("/C", []() {
    digitalWrite(LED4, HIGH);
    handleRoot();
  });
  server.on("/D", []() {
    digitalWrite(LED4, LOW);
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

  static bool prevLED1SwitchState = LOW;
  static bool prevLED2SwitchState = LOW;
  static bool prevLED3SwitchState = LOW;
  static bool prevLED4SwitchState = LOW;

  // Read current switch states
  bool currentLED1SwitchState = digitalRead(LED1Switch);
  bool currentLED2SwitchState = digitalRead(LED2Switch);
  bool currentLED3SwitchState = digitalRead(LED3Switch);
  bool currentLED4SwitchState = digitalRead(LED4Switch);

  // Check for LED1 toggle
  if (currentLED1SwitchState == LOW && prevLED1SwitchState == HIGH) {
    digitalWrite(LED1, !digitalRead(LED1)); // Toggle LED1 state
//    saveToEEPROM(LED1_STATE_ADDR, digitalRead(LED1)); // Save state to EEPROM
    delay(200); // Debounce delay
  }
  prevLED1SwitchState = currentLED1SwitchState;

  // Check for LED2 toggle
  if (currentLED2SwitchState == LOW && prevLED2SwitchState == HIGH) {
    digitalWrite(LED2, !digitalRead(LED2)); // Toggle LED2 state
    //saveToEEPROM(LED2_STATE_ADDR, digitalRead(LED2)); // Save state to EEPROM
    delay(200); // Debounce delay
  }
  prevLED2SwitchState = currentLED2SwitchState;

  // Check for LED3 toggle
  if (currentLED3SwitchState == LOW && prevLED3SwitchState == HIGH) {
    digitalWrite(LED3, !digitalRead(LED3)); // Toggle LED3 state
   // saveToEEPROM(LED3_STATE_ADDR, digitalRead(LED3)); // Save state to EEPROM
    delay(200); // Debounce delay
  }
 // prevLED3SwitchState = currentLED3SwitchState;

  // Check for LED4 toggle
  if (currentLED4SwitchState == LOW && prevLED4SwitchState == HIGH) {
    digitalWrite(LED4, !digitalRead(LED4)); // Toggle LED4 state
  //  saveToEEPROM(LED4_STATE_ADDR, digitalRead(LED4)); // Save state to EEPROM
    delay(200); // Debounce delay
  }
  prevLED4SwitchState = currentLED4SwitchState;
}
