/*
    BLE_MIDI Example by neilbags 
    https://github.com/neilbags/arduino-esp32-BLE-MIDI
    
    Based on BLE_notify example by Evandro Copercini.
    Creates a BLE MIDI service and characteristic.
    Once a client subscibes, send a MIDI message every 2 seconds
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <driver/adc.h>

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"


uint8_t midiPacket[] = {
   0x80,  // header
   0x80,  // timestamp, not implemented 
   0x00,  // status
   0x3c,  // 0x3c == 60 == middle c
   0x00   // velocity
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);

  BLEDevice::init("MIDIwithPitchBend");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();  // new BLEServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // Create the BLE Service
  BLEService *pService = pServer->createService(BLEUUID(MIDI_SERVICE_UUID));
  
  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      BLEUUID(MIDI_CHARACTERISTIC_UUID),
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_WRITE_NR
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
}

void loop() {
  if (deviceConnected) {
   // note down
   midiPacket[2] = 0x90; // note down, channel 0
   midiPacket[3] = 0x3C; // middle-C
   midiPacket[4] = 127;  // velocity
   pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
   pCharacteristic->notify();

   // play note for 800ms while sending pitchBend messages
   for(int i=0;i<80;i++){
     adc1_config_width(ADC_WIDTH_BIT_12);   //Range 0-4095 
     adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);  //ADC_ATTEN_DB_0 = 0-3.3V
     int val = adc1_get_raw( ADC1_CHANNEL_0 );  // setup ADC
     uint16_t sensor = (analogRead(36)) << 2; // Read analog, convert from a 12-bit number to a 14-bit number by shifting it 2 bits to the left (adds 2 padding zeros to the right).
     unsigned char LSB = sensor & 0x7F;
     unsigned char MSB = (sensor >> 7) & 0x7F;
     midiPacket[2] = 0xE0; // pitch bend, channel 0
     midiPacket[3] = LSB; // Least significant 7-bits of pitch-bend value
     midiPacket[4] = MSB; // Most significant 7-bits of pitch-bend value
     pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
     pCharacteristic->notify();
     delay(10);
   }
   // note up
   midiPacket[2] = 0x80; // note up, channel 0
   midiPacket[3] = 0x3C; // middle-C
   midiPacket[4] = 0;    // velocity
   pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
   pCharacteristic->notify();

   delay(200);
  }
}
