#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <vector>

#define DHTPIN 4
#define DHTTYPE 11

using namespace std;

DHT dht(DHTPIN, DHTTYPE);

const char ssid[] = "Wifi_Name";
const char password[] = "Wifi_Password";

AsyncWebServer server(80);

vector<float> temperatureData;
vector<float> humidityData;
void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String html = "<!DOCTYPE html><html><head><title>DHT11 Data</title></head><body>";
    html += "<h1>Recent Measurements</h1><table border='1'><tr><th>Temperature (°C)</th><th>Humidity (%)</th></tr>";
    for (size_t i = 0; i < temperatureData.size(); ++i) {
      html += "<tr><td>" + String(temperatureData[i]) + "</td><td>" + String(humidityData[i]) + "</td></tr>";
    
    }
    html += "</table></body></html>";
    request->send(200, "text/html", html); });
  server.begin();
  
}
void loop()
{
  static unsigned long LastRead = 0;
  if (millis() - LastRead > 5000)
  {
    LastRead = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h))
    {
      Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%\n", t, h);
      temperatureData.push_back(t);
      humidityData.push_back(h);

      if (temperatureData.size() > 10)
      {
        temperatureData.erase(temperatureData.begin());
        humidityData.erase(humidityData.begin());
      }
    }
    else
    {
      Serial.println("Failed to read from DHT sensor.");
    }
  }
}
