#include <Arduino_JSON.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Pinger.h>


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("> Booting...");
  String status = "OK";
  static uint32_t const time_delay = 1 * 60 * 60 / 2;

  Adafruit_BMP280 bmp{};
  if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
    Serial.println("! Failed to connect to BMP280");
    status = "NO_SENSOR";
  }

  bmp.setSampling(Adafruit_BMP280::MODE_FORCED, Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  float temperature = -273.0;
  float pressure = -1.0;

  if (bmp.takeForcedMeasurement()) {
    temperature = bmp.readTemperature();
    pressure = bmp.readPressure();

    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(" *C");
    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(" Pa");
  } else {
    status = "NO_MEASURE";
  }

// Connect to Wifi
#include "wifi_credentials.h"
  WiFi.begin(ssid, password);
  Serial.print("> Connecting to WIFI: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("> Connected: ");
  Serial.println(WiFi.localIP());

  WiFiUDP ntpUDP{};
  NTPClient timeClient(ntpUDP, "br.pool.ntp.org");
  timeClient.begin();
  timeClient.update();
  time_t const timestamp = timeClient.getEpochTime();
  Serial.print("> Current timestamp: ");
  Serial.println((long)timestamp);

  // Post to server
  WiFiClient client{};
  HTTPClient http{};
  http.begin(client, "http://192.168.1.107:9090/");
  http.addHeader("Content-Type", "application/json");

  JSONVar doc{};
  doc["sensor"] = "bmp280";
  doc["time"] = (long)timestamp;
  doc["temperature"] = temperature;
  doc["pressure"] = pressure;
  doc["status"] = status;

  String payload = JSON.stringify(doc);
  Serial.print("> Payload: ");
  Serial.println(payload);
  uint8_t attempt = 5;
  delay(500);
  while (attempt--) {
    int response = http.POST(payload);
    if (response != 200) {
      Serial.print("! Failed to POST data! ");
      Serial.println(response);
    } else {
      Serial.println("> 200 OK");
      break;
    }
  }

  http.end();
  client.stop();
  // Sleep 30s
  ESP.deepSleep(time_delay * 1E6);
}

void loop() {}
