// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

#define DHTPIN 5     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define ID "krypgrund-right"

#define FIVE_MINUTES_IN_MILLI (1000 * 60 * 5)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = STASSID;
const char* password = STAPSK;
const char* id = ID;
const int capacity = JSON_OBJECT_SIZE(3);

WiFiClient espClient;
WiFiUDP Udp;

IPAddress ip(255, 255, 255, 255);
unsigned int udpPort = 1900;

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  randomSeed(micros());

  dht.begin();
}

void loop() {
  StaticJsonDocument<capacity> doc;
  char output[256];

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    doc["error"] = "failed to read from DHT sensor";
  } else {
    doc["humidity"] = h;
    doc["temperature"] = t;
  }
  doc["id"] = id;

  serializeJson(doc, output);
  Serial.print("json: ");
  Serial.println(output);

  Udp.beginPacket(ip, udpPort);
  Udp.write(strcat(output, "\n"));
  Udp.endPacket();

  delay(FIVE_MINUTES_IN_MILLI);
}
