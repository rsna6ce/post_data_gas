// ================================================
// ESP32 Example: POST sine and cosine values to Google Apps Script
// Arduino IDE + ESP32 Board
// Required libraries: WiFi.h and HTTPClient.h (included by default)
// ================================================

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials (change these to your own)
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Google Apps Script Web App URL
// (Issued after deploying as Web App → "Anyone" access)
const char* serverUrl = "https://script.google.com/macros/s/xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx/exec";

// Project name (must match PROJECT_NAME in GAS)
const char* projectName = "PROJECT-XXX";

// Send data every X milliseconds
const unsigned long SEND_INTERVAL_MS = 60000UL;  // ≈ 60 points per cycle

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

unsigned long latestSendTime = SEND_INTERVAL_MS;
void loop() {
  unsigned long now = millis();

  // Send data periodically
  if (now - latestSendTime >= SEND_INTERVAL_MS) {
    latestSendTime = now;

    // Calculate elapsed seconds since boot
    float elapsedSeconds = (float)now / 1000.0f;

    // Angle in radians (one full cycle = 3600 seconds)
    const unsigned long CYCLE_SECONDS = 3600UL;
    float angle = (elapsedSeconds / CYCLE_SECONDS) * TWO_PI;
    float sineValue   = sin(angle);
    float cosineValue = cos(angle);
    sendData(sineValue, cosineValue);
  }

  // You can add other tasks here (no blocking delay needed)
}

// ================================================
// Send values to GAS via POST
// ================================================
void sendData(float sinValue, float cosValue) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }
  WiFiClientSecure client;
  client.setInsecure();  // Skip certificate verification (for development / simple use)

  HTTPClient http;

  if (!http.begin(client, serverUrl)) {
    Serial.println("Failed to begin HTTP connection");
    return;
  }
  http.addHeader("Content-Type", "application/json");

  // Build JSON payload manually
  String jsonPayload = "{"
    "\"project_name\":\"" + String(projectName) + "\","
    "\"headers\":[\"sin\",\"cos\"],"
    "\"datas\":[" + String(sinValue, 4) + "," + String(cosValue, 4) + "],"
    "\"max_count\":1440"
  "}";

  Serial.print("Sending JSON: ");
  Serial.println(jsonPayload);

  int httpCode = http.POST(jsonPayload);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.printf("HTTP Response code: %d\n", httpCode);
    Serial.print("Response: ");
    Serial.println(response);
  } else {
    Serial.printf("POST failed, error: %d\n", httpCode);
  }

  http.end();
}
