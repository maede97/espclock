#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_BME280.h>
#include "MyClock.h"

#define TFT_MOSI 21
#define TFT_SCLK 20
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8

#define BME_SDA 3
#define BME_SCL 2
#define BME_GND 1
#define BME_VDD 0

#define SERVICE_UUID "55c02300-ea68-4a19-a11d-b117f4e2f2c1"
BLEService bleService(SERVICE_UUID);
BLECharCharacteristic bleHoursChar("0001", BLEWrite | BLERead | BLENotify);
BLEDescriptor bleHoursDescriptor("2901", "Current Hours");
BLECharCharacteristic bleMinutesChar("0002", BLEWrite | BLERead | BLENotify);
BLEDescriptor bleMinutesDescriptor("2901", "Current Minutes");
BLECharCharacteristic bleSecondsChar("0003", BLEWrite | BLERead | BLENotify);
BLEDescriptor bleSecondsDescriptor("2901", "Current Seconds");

MyClock myclock;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
Adafruit_BME280 bme;

void setup() {

  BLE.begin();
  BLE.setLocalName("espclock");
  BLE.setAdvertisedService(bleService);

  bleService.addCharacteristic(bleHoursChar);
  bleHoursChar.addDescriptor(bleHoursDescriptor);
  bleService.addCharacteristic(bleMinutesChar);
  bleMinutesChar.addDescriptor(bleMinutesDescriptor);
  bleService.addCharacteristic(bleSecondsChar);
  bleSecondsChar.addDescriptor(bleSecondsDescriptor);

  BLE.addService(bleService);
  BLE.advertise();

  myclock.setHours(0);
  myclock.setMinutes(0);
  myclock.setSeconds(0);

  tft.initR(INITR_MINI160x80_PLUGIN);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);

  // set up ground for BME280
  pinMode(BME_GND, OUTPUT);
  digitalWrite(BME_GND, LOW);

  // set up power for BME280
  pinMode(BME_VDD, OUTPUT);
  digitalWrite(BME_VDD, HIGH);

  Wire.setPins(BME_SDA, BME_SCL);
  bme.begin(0x76);
}

void clockUpdate(Adafruit_GFX* tft) {
  std::tuple<bool, bool, bool> updated = myclock.update();

  if (std::get<0>(updated)) {
    bleSecondsChar.writeValue(myclock.getSeconds());
    tft->fillRect(110, 25, 45, 30, ST7735_BLACK);
  }
  
  if(std::get<1>(updated)) {
    bleMinutesChar.writeValue(myclock.getMinutes());
    tft->fillRect(60, 25, 45, 30, ST7735_BLACK);
  }
  if(std::get<2>(updated)) {
     bleHoursChar.writeValue(myclock.getHours());
     tft->fillRect(8, 25, 45, 30, ST7735_BLACK);
  }

  if (std::get<0>(updated) || std::get<1>(updated) || std::get<2>(updated)) {
    tft->setFont();
    tft->setTextSize(3);
    tft->setCursor(10, 30);
    tft->printf("%02d:%02d:%02d", myclock.getHours(), myclock.getMinutes(), myclock.getSeconds());

    tft->setTextSize(1);

    tft->fillRect(10, 10, 100, 10, ST7735_BLACK);
    tft->setCursor(10, 10);
    tft->printf("Temp: %.2fC", bme.readTemperature());
    
    tft->fillRect(10, 60, 100,10, ST7735_BLACK);
    tft->setCursor(10, 60);
    tft->printf("Humidity: %.2f%%", bme.readHumidity());
  }
}


void loop() {
  BLEDevice central = BLE.central();
  if(central) {
    while(central.connected()) {
      clockUpdate(&tft);
      if(bleHoursChar.written()) {
        myclock.setHours(bleHoursChar.value());
        tft.fillRect(8, 25, 45, 30, ST7735_BLACK);
      }
      if(bleMinutesChar.written()) {
        myclock.setMinutes(bleMinutesChar.value());
        tft.fillRect(60, 25, 45, 30, ST7735_BLACK);
      }
      if(bleSecondsChar.written()) {
        myclock.setSeconds(bleSecondsChar.value());
        // rect fill not needed as is done every second.
      }
    }
  }

  clockUpdate(&tft);
}
