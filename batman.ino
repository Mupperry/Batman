/*
Based on AdvancedWebServer example
 */
#define PIN 4

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

const char *ssid = "AP";
const char *password = "PBAPWWW123";

ESP8266WebServer server ( 80 );
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);

const int led = 13;

void handleRoot() {
	int sec = (millis() / 1000);
	int min = (sec / 60);
  int rawValue = analogRead(A0);
  float voltage = float(rawValue)/1024.0 * 10.83 ; // For 1.2 V reference, maybe not correct
  
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
    response += "<p>Raw value: ";
    response += rawValue;
    response += "</P>";
    response += "<p>Voltage: ";
    response += voltage;
    response += "</P>";
  response += "</body>";
response += "</html>";
  
	server.send ( 200, "text/html", response );
	digitalWrite ( led, 0 );
}

void setup ( void ) {
	pinMode ( led, OUTPUT );
	digitalWrite ( led, 0 );
  strip.begin();
  strip.show();
  colorWipe(strip.Color(10, 0, 0),20);
	Serial.begin ( 115200 );
	WiFi.begin ( ssid, password );
	Serial.println ( "" );

	// Wait for connection
	while ( WiFi.status() != WL_CONNECTED ) {
		delay ( 500 );
		Serial.print ( "." );

   
	}

	Serial.println ( "" );
	Serial.print ( "Connected to " );
	Serial.println ( ssid );
	Serial.print ( "IP address: " );
	Serial.println ( WiFi.localIP() );

	server.on ( "/", handleRoot );
	server.on ( "/inline", []() {
		server.send ( 200, "text/plain", "this works as well" );
	} );
	server.begin();
	Serial.println ( "HTTP server started" );
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void loop ( void ) {
	server.handleClient();
  Serial.println(analogRead(A0));
  delay(500);
  yield();
}


