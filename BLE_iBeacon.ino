/*
   Based on 31337Ghost's reference code from https://github.com/nkolban/esp32-snippets/issues/385#issuecomment-362535434
   which is based on pcbreflux's Arduino ESP32 port of Neil Kolban's example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
*/

/*
   This code is test on ESP32-WROOM board
   Create a BLE server that will send periodic iBeacon frames.
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create advertising data
   3. Start advertising.
   4. wait
   5. Stop advertising.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLEBeacon.h>

#define DEVICE_NAME            "CCTV1_TOKEN"
#define BEACON_UUID_DISARM_REV     "xxxxxxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxx"
#define BEACON_UUID_TEST_REV       "yyyyyyyyyyyy-yyyy-yyyy-yyyy-yyyyyyyy"
#define CCTV1_ID                "wwwww"
#define MINOR                   "zzzzz"

BLEServer *pServer = nullptr;
volatile uint8_t previous_mode, current_mode;
const int test_pin = 4;


void init_beacon(uint32_t mode) {

  if (BLEDevice::getInitialized()){
    BLEDevice::deinit();
  }
  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();

  BLEAdvertising* pAdvertising;
  pAdvertising = pServer->getAdvertising();
  pAdvertising->stop();
  // iBeacon
  BLEBeacon myBeacon;
  myBeacon.setManufacturerId(0x4c00);
  myBeacon.setMajor(CCTV1_ID);
  myBeacon.setMinor(MINOR);
  myBeacon.setSignalPower(0xc5);
  myBeacon.setProximityUUID(BLEUUID(mode == 0 ? BEACON_UUID_DISARM_REV : BEACON_UUID_TEST_REV));
  

  BLEAdvertisementData advertisementData;
  advertisementData.setFlags(0x1A);
  advertisementData.setManufacturerData(myBeacon.getData());
  pAdvertising->setAdvertisementData(advertisementData);

  pAdvertising->start();
}

void update_beacon() {
  //current_mode = touchRead(T0)>50 ? 0 : 1;
  //Serial.println(touchRead(T0));
  current_mode = digitalRead(test_pin);
  Serial.println(current_mode);
  if (current_mode != previous_mode){
    init_beacon(current_mode);
    previous_mode = current_mode;
    Serial.print("Current mode: ");
    Serial.println(current_mode == 0 ? "DISARM" : "TEST");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing...");
  Serial.flush();
  
  pinMode(test_pin, INPUT);

  current_mode = digitalRead(test_pin);
  init_beacon(current_mode);
  previous_mode = current_mode;

  Serial.println("iBeacon + service defined and advertising!");
}

void loop() {
  update_beacon();
  delay(100);
}
