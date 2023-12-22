/*
Author: Mustafa DUT

https://www.linkedin.com/in/mustafa-dut/

*/


#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include "SDL_Arduino_INA3221.h"
#include <ArduinoJson.h>
#include <U8g2lib.h>

// in3221 object create
SDL_Arduino_INA3221 ina3221;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

// Replace with your network credentials
const char *ssid = "xxx";
const char *password = "xxx";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getChannel1()
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float watt = 0;

  busvoltage = ina3221.getBusVoltage_V(1);
  shuntvoltage = ina3221.getShuntVoltage_mV(1);
  current_mA = -ina3221.getCurrent_mA(1); // minus is to get the "sense" right.   - means the battery is charging, + that it is discharging
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  // loadvoltage = loadvoltage + 0.4;
  watt = loadvoltage * current_mA / 1000;

  // Create a JSON object
  StaticJsonDocument<200> doc;
  doc["voltage"] = loadvoltage;
  doc["current"] = current_mA;
  doc["power"] = watt;

  // Convert the JSON object to a string
  String output;
  serializeJson(doc, output);

  return output;
}

String getChannel2()
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float watt = 0;

  busvoltage = ina3221.getBusVoltage_V(2);
  shuntvoltage = ina3221.getShuntVoltage_mV(2);
  current_mA = -ina3221.getCurrent_mA(2); // minus is to get the "sense" right.   - means the battery is charging, + that it is discharging
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  // loadvoltage = loadvoltage + 0.4;
  watt = loadvoltage * current_mA / 1000;

  // Create a JSON object
  StaticJsonDocument<200> doc;
  doc["voltage"] = loadvoltage;
  doc["current"] = current_mA;
  doc["power"] = watt;

  // Convert the JSON object to a string
  String output;
  serializeJson(doc, output);
  Serial.println(loadvoltage);
  return output;
}

String getChannel3()
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float watt = 0;

  busvoltage = ina3221.getBusVoltage_V(3);
  shuntvoltage = ina3221.getShuntVoltage_mV(3);
  current_mA = -ina3221.getCurrent_mA(3); // minus is to get the "sense" right.   - means the battery is charging, + that it is discharging
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  // loadvoltage = loadvoltage + 0.4;
  watt = loadvoltage * current_mA / 1000;

  // Create a JSON object
  StaticJsonDocument<200> doc;
  doc["voltage"] = loadvoltage;
  doc["current"] = current_mA;
  doc["power"] = watt;

  // Convert the JSON object to a string
  String output;
  serializeJson(doc, output);

  return output;
}

String processor(const String &var)
{
  Serial.println(var);
  if (var == "getChannel1")
  {
    return getChannel1();
  }
  else if (var == "getChannel2")
  {
    return getChannel2();
  }
  else if (var == "getChannel3")
  {
    return getChannel3();
  }
}

void setup()
{

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  u8g2.begin();
  u8g2.clearBuffer();                 // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB12_tr); // choose a suitable font
  u8g2.setCursor(0, 24);
  u8g2.print("Power Meter");
  // u8g2.drawStr(0,32,"Hello World!");  // write something to the internal memory
  u8g2.sendBuffer(); // transfer internal memory to the display

  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize the sensor INA3221
  ina3221.begin();

  // Initialize SPIFFS
  if (!SPIFFS.begin())
  {
    Serial.println("[INFO] : An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("[INFO] : Connecting to WiFi..");
  }

  // ina3221 Manufactures ID
  Serial.print("[INFO] : Manufactures ID=0x");
  int MID;
  MID = ina3221.getManufID();
  Serial.println(MID, HEX);

  u8g2.clearBuffer();

  Serial.print("[INFO] -> IP : ");
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  digitalWrite(LED_BUILTIN, HIGH);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 24);
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style.css", "text/css"); });

  // server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send_P(200, "text/plain", getTemperature().c_str()); });

  server.on("/channel1", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String getChannel1Json = getChannel1();
    request->send(200, "application/json", getChannel1Json); });

  server.on("/channel2", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String getChannel2Json = getChannel2();
    request->send(200, "application/json", getChannel2Json); });

  server.on("/channel3", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String getChannel3Json = getChannel3();
    request->send(200, "application/json", getChannel3Json); });

  // Start server
  server.begin();
}

void loop()
{
}
