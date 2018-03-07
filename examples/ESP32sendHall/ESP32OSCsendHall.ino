/*---------------------------------------------------------------------------------------------
  Open Sound Control (OSC) library for the ESP8266/ESP32
  Example for sending messages from the ESP8266/ESP32 to a remote computer
  The example is sending "hello, osc." to the address "/test".
  This example code is in the public domain.
--------------------------------------------------------------------------------------------- */
#define ESP32
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>


WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192,168,4,2);         //default IP, will change with received udp
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)
const char *ssid = "ESP32-OSC";             //BS SSID

int hallSensor;

void setup() {
    Serial.begin(115200);

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
