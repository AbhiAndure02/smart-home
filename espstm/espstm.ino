#include <Wire.h>
#include <ESP8266WiFi.h>

const char* ssid = "TechInfoSync";
const char* password = "Admin@1234";

#define STM_I2C_ADDRESS 0x08 // I2C address of STM

WiFiServer server(80);

void setup() {
  Serial.begin(9600); // Debugging
  Wire.begin();       // Initialize I2C as master
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println(request);

    // Control LEDs based on the URL
    if (request.indexOf("/LED1_ON") != -1) sendCommand("LED1_ON");
    if (request.indexOf("/LED1_OFF") != -1) sendCommand("LED1_OFF");
    if (request.indexOf("/LED2_ON") != -1) sendCommand("LED2_ON");
    if (request.indexOf("/LED2_OFF") != -1) sendCommand("LED2_OFF");
    if (request.indexOf("/LED3_ON") != -1) sendCommand("LED3_ON");
    if (request.indexOf("/LED3_OFF") != -1) sendCommand("LED3_OFF");
    if (request.indexOf("/LED4_ON") != -1) sendCommand("LED4_ON");
    if (request.indexOf("/LED4_OFF") != -1) sendCommand("LED4_OFF");
    if (request.indexOf("/LED5_ON") != -1) sendCommand("LED5_ON");
    if (request.indexOf("/LED5_OFF") != -1) sendCommand("LED5_OFF");

    // Get switch states
    if (request.indexOf("/SWITCH_STATE") != -1) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");

      String switchStates = getSwitchStates();
      client.println("<h1>Switch States</h1>");
      client.println("<p>" + switchStates + "</p>");
    } else {
      // Default webpage
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("");
      client.println("<h1>LED Control</h1>");
      for (int i = 1; i <= 5; i++) {
        client.println("<a href=\"/LED" + String(i) + "_ON\">LED " + String(i) + " ON</a><br>");
        client.println("<a href=\"/LED" + String(i) + "_OFF\">LED " + String(i) + " OFF</a><br>");
      }
      client.println("<a href=\"/SWITCH_STATE\">Get Switch States</a><br>");
    }

    client.stop();
  }
}

// Send I2C command to STM
void sendCommand(const char* command) {
  Wire.beginTransmission(STM_I2C_ADDRESS);
  Wire.write(command);
  byte error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("Command sent successfully: " + String(command));
  } else {
    Serial.println("Error sending command. I2C error code: " + String(error));
  }
}

// Get switch states from STM
String getSwitchStates() {
  Wire.requestFrom(STM_I2C_ADDRESS, 32); // Request up to 32 bytes
  String states = "";
  while (Wire.available()) {
    char c = Wire.read();
    states += c;
  }
  if (states.length() > 0) {
    Serial.println("Received switch states: " + states);
  } else {
    Serial.println("No data received from STM.");
  }
  return states;
}
