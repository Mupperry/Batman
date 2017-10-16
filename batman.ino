/*
 Batman
 Battery monitoring using ESP8266

 BAT_VMAX set for 3S Li-Po, 
 Max ADC voltage is 1.0 V in datasheet but may be different in practice (1.23 V)
 */

#define PIXEL_PIN 4 
#define BAT_VMAX 13.3

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "AP";
const char* password = "PBAPWWW123";
unsigned int averageVoltage = 0;
unsigned int readings[5] = {0, 0, 0, 0, 0}; //stores last 5 readings for averaging
unsigned int readIndex = 0;

// Create Webserver and NeoPixel strip
ESP8266WebServer server ( 80 );
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"abc");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  strip.begin();
  strip.show(); // Initialize all pixels to off-state
  colorWipe(strip.Color(10, 0, 0),20);
  
}

void handleRoot() {
  int sec = (millis() / 1000);
  int min = (sec / 60);
  
  String response = "<html>";
  response += "<head>";
    response += "<meta http-equiv='refresh' content='3'/>";
    response += "<title>ESP8266 Demo</title>";
    response += "<style>";
     response += "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
    response += "</style>";
  response += "</head>";
  response += "<body>";
    response += "<h1>Battery Monitor</h1>";
    response += "<p>Uptime: ";
    response += min%60;
    response += ":";
    response += sec%60;
    response += "</p>";
    response += "<p>Voltage: ";
    response += averageVoltage;
    response += "</P>";
  response += "</body>";
response += "</html>";
  
  server.send ( 200, "text/html", response );
}

void updateVoltage(){
  readings[readIndex] = float(analogRead(0))/1024.0 * BAT_VMAX;
  float temp = 0;
  for (int i = 0; i < sizeof(readings)/sizeof(unsigned int); i++){ //sizeof() reads bytes, divide by size of variable for number of elements
    temp += readings[i];
  }
  averageVoltage = temp/float(sizeof(readings)/sizeof(unsigned int));
  readIndex++;
}

bool ota_flag = true;
unsigned int elapsed_time = 0;

void loop() {
  if(ota_flag){
    while(elapsed_time < 15000){
      ArduinoOTA.handle();
      elapsed_time = millis();
      delay(5);
    }
    ota_flag = false;
  }
  updateVoltage();
  server.handleClient();
  yield();
  delay(50);
}
