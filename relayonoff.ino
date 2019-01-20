#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "ssid";
const char* password = "pass";

bool relayOn = true;

ESP8266WebServer server ( 80 );
HTTPClient http;

/* ===================================================================== */

void setup ( void ) {

  // 9600 speed is needed for HW-655 to work
  Serial.begin(9600);
  pinMode(  2 , INPUT_PULLUP);

  WiFi.mode ( WIFI_STA );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 1000 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  server.on("/", handleRoot);
  server.on("/1", handleOpen); //relay on
  server.on("/0", handleClose);//relay off
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println ( "HTTP server started" );
}

/* ===================================================================== */

void loop ( void ) {

  server.handleClient();
  toggle();
  delay(2000);

}


/*===================================================================
                          ACTIONS
  ===================================================================*/


void turnOn() {
  Serial.write("\xa0\x01"); // byte sequence for opening relay
  Serial.write(0x00);   // apparently because of the 0x00
  Serial.write(0xa1);   // you need to send on multiple lines
  relayOn = true;
}

void turnOff() {
  Serial.write("\xa0\x01\x01\xa2"); // byte sequence for closing relay
  relayOn = false;
}

/*===================================================================
                          STATUS
  ===================================================================*/

void toggle() {
  if (relayOn) {
      turnOn();
  }
  else {
      turnOff();
  }
}

/*===================================================================
                          SERVER URLs
  ===================================================================*/

void handleRoot() {
  server.send(200, "text/plain", "Hello, the Garage Door gateway is ready");
}

void handleNotFound() {
  server.send(404, "text/plain", "Nut Found");
}

void handleOpen() {
  // only open the door if it was closed
   turnOff();
  server.send ( 200, "text/plain", "Relay ON" );
}

void handleClose() {
  // only close the door if it is open
  turnOn();
  server.send ( 200, "text/plain", "Relay ON" );
}
