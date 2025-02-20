#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "Hi";
const char* password = "123456789";

// LED pins
const int ledPins[] = {2, 4, 5, 18, 19};

// Button pins
const int button1 = 13;
const int button2 = 14;
const int button3 = 15;

WebServer server(80);

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>ESP32 LED Control</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; }";
  html += ".container { display: flex; flex-direction: column; align-items: center; gap: 20px; margin-top: 50px; }";
  html += ".box { background: white; padding: 20px; width: 250px; border-radius: 10px; box-shadow: 0px 4px 6px rgba(0,0,0,0.2); }";
  html += "button { width: 100%; padding: 15px; font-size: 18px; border: none; cursor: pointer; border-radius: 5px; background-color: #007BFF; color: white; transition: 0.3s; }";
  html += "button:hover { background-color: #E52020; }";
  html += "</style></head><body>";

  html += "<h1>ESP32 LED Control</h1>";
  html += "<div class='container'>";
  html += "<div class='box'><button onclick=\"fetch('/all_on').then(response => alert('All LEDs ON!'))\">All LEDs ON</button></div>";
  html += "<div class='box'><button onclick=\"fetch('/alternate').then(response => alert('Alternating LEDs!'))\">Alternate LEDs</button></div>";
  html += "<div class='box'><button onclick=\"fetch('/center_out').then(response => alert('Center to Edge LEDs!'))\">Center to Edge</button></div>";
  html += "</div>";
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Turn all LEDs ON
void handleAllOn() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  server.send(200, "text/plain", "All LEDs are ON");
}

// Alternating LED pattern: 1, 3, 5, 2, 4, loops twice then turns off
void handleAlternate() {
  int sequence[] = {0, 2, 4, 1, 3}; // LED order: 1, 3, 5, 2, 4
  int sequenceLength = sizeof(sequence) / sizeof(sequence[0]);

  for (int loop = 0; loop < 2; loop++) { // Run twice
    for (int i = 0; i < sequenceLength; i++) {
      digitalWrite(ledPins[sequence[i]], HIGH);
      delay(500); // Delay for visibility
      digitalWrite(ledPins[sequence[i]], LOW);
    }
  }

  // Ensure all LEDs are off after two cycles
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[i], LOW);
  }

  server.send(200, "text/plain", "Alternating LED sequence activated and turned off after 2 cycles");
}


void handleCenterOut() {
    int order[][2] = {{2, 2}, {1, 3}, {0, 4}}; // LED pairs from center outward
    
    for (int loop = 0; loop < 2; loop++) { // Run twice
        for (int i = 0; i < 3; i++) {
            digitalWrite(ledPins[order[i][0]], HIGH);
            digitalWrite(ledPins[order[i][1]], HIGH);
            delay(500);
            digitalWrite(ledPins[order[i][0]], LOW);
            digitalWrite(ledPins[order[i][1]], LOW);
        }
        delay(500);
        
        for (int i = 2; i >= 0; i--) {
            digitalWrite(ledPins[order[i][0]], HIGH);
            digitalWrite(ledPins[order[i][1]], HIGH);
            delay(500);
            digitalWrite(ledPins[order[i][0]], LOW);
            digitalWrite(ledPins[order[i][1]], LOW);
        }
    }
    
    for (int i = 0; i < 5; i++) {
        digitalWrite(ledPins[i], LOW);
    }
    
    server.send(200, "text/plain", "Center to Edge LEDs activated and turned off after 2 cycles");
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/all_on", handleAllOn);
  server.on("/alternate", handleAlternate);
  server.on("/center_out", handleCenterOut);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (digitalRead(button1) == LOW) {
    handleAllOn();
    delay(500);
  }
  if (digitalRead(button2) == LOW) {
    handleAlternate();
    delay(500);
  }
  if (digitalRead(button3) == LOW) {
    handleCenterOut();
    delay(500);
  }
}
