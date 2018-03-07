/*---------------------------------------------------------------------------------------------
  Open Sound Control (OSC) library for the ESP8266/ESP32
  Example for sending messages from the ESP8266/ESP32 to a remote computer
  The example is sending "hello, osc." to the address "/test".
  This example code is in the public domain.
--------------------------------------------------------------------------------------------- */
#define ESP32wOLED //use Arduino's "Manage Libraries" function to search and add "ESP8266 and ESP32 Oled driver for SSD1306 display"
#define ESP32
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>

#if defined(ESP32wOLED)
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
SSD1306  display(0x3c, 5, 4);
#endif


WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192,168,4,2);         //default IP, will change with received udp
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)
const char *ssid = "ESP32-OSC";             //BS SSID

int hallSensor;

#if defined(ESP32wOLED)
void printBuffer(void) {
  // Initialize the log buffer
  // allocate memory to store 8 lines of text and 30 chars per line.
  display.setLogBuffer(5, 30);

  // Some test data
  const char* test[] = {
    "Join my network called",
    "ESP32-OSC",
    "---------",
    "You can receive OSC",
    "packets on port 9999",
  };

  for (uint8_t i = 0; i < 5; i++) {
    display.clear();
    // Print to the screen
    display.println(test[i]);
    // Draw it to the internal screen buffer
    display.drawLogBuffer(0, 0);
    // Display it on the screen
    display.display();
    delay(500);
  }
}
#endif



void setup() {
    Serial.begin(115200);
    #if defined(ESP32wOLED)
    display.init();
    display.flipScreenVertically();
    display.setContrast(255);
    printBuffer();
    #endif
    
    // Create a WiFi network
    WiFi.softAP(ssid);
    IPAddress myIP = WiFi.softAPIP();

    Serial.println("IP address: ");
    Serial.println(myIP);

    Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.print("Sending to port: ");
#ifdef ESP32
    Serial.println(outPort);
#else
    Serial.println(Udp.outPort());
#endif

}

void loop() {
    hallSensor = hallRead();
    OSCMessage msg("/hall"); //OSC message compilation
    msg.add(hallSensor);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty(); // free space occupied by message
    delay(5);
}
