// https://docs.arduino.cc/tutorials/projects/lorawan-farming-with-mkr-wan-1310
// impiega Node-RED collegato a TTN :-)
// per hardware PULCHRA FULL MKRWAN 0.3
// 
// Ingressi:
// Sensore di temperatura esterna: PT100 4 fili con MAX31865 (SPI)
// Sensore di temperatura e di umidità: BME280 (I2C)
// Sensore di temperatura interna: LM35
// Anemometro (impulsi)
// Direzione vento (analogico)
// Sensore bagnatura foglia (on/off)
// Pluviometro (impulsi) con riscaldatore
// Livello batteria +12V
//
// Uscite:
// display oled SSD1306 128x64 (I2C)
// comando ventola +12VDC areazione interna, on/off o pwm


#include <Arduino.h>
#include <ArduinoLowPower.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MAX31865.h>
#include <U8g2lib.h>
#include <anemometro.h>
#include <ArduinoJson.h>
#include <MKRWAN.h>
#include <arduino_secrets.h>
#include <LM35.h>
#include <misure.h>
#include <ttn.h>

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

#define PT100_4W
//#define PT1000_3W
//#define _SLEEP

#define LORA_PUBLISH_PERIOD (2 * 60 * 1000)

#define SENS_FOGLIA (0)
#define OUT_VENTOLA (4)
#define PLUVIOMETRO (5)
#define ANE_PIN (6)
#define CS_RTD (7)

//#define BMP_SCK (13)
//#define BMP_MISO (12)
//#define BMP_MOSI (11)
//#define BMP_CS (10)

#define TEMP_PIN (A0)
#define DIR_PIN (A1)
#define LIV_BATTERIA (A2)

// LoRaWAN link
bool isOnline = false;

// display
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// sensore LM35
LM35 lm35(TEMP_PIN, AR_INTERNAL1V65);

// BME280 on I2C bus
Adafruit_BME280 bme;

// MAX31865 RTD signal acquisition
// use hardware SPI, just pass in the CS pin
Adafruit_MAX31865 thermo = Adafruit_MAX31865(CS_RTD);

#ifdef PT100_4W
// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF 430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL 100.0
#endif

#ifdef PT1000_3W
// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF 4300.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL 1000.0
#endif

// buffer for printing
char msg[50];

TMisura misura;

void lettura(TMisura *m);

// buffer del data packet
char ttnDataPacket[PKTSIZE];

// LoRaWAN radio modem
LoRaModem modem;

// JSON object
StaticJsonDocument<128> doc;
// declare a buffer to hold the result
char JSONoutput[128];

// time keeper for periodic LoRaWAN publishing
uint32_t lastTime = 0;

// --- setup function ---------------------------------------------------------
void setup()
{
  Serial.begin(115200);
//    while (!Serial)
      delay(1000); // wait for native usb

  // anemometro
  anemometroSetup(ANE_PIN, 2.44, 0.5, 100.0);

  // display
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 16, "Pulchra");
  u8g2.drawStr(20, 32, "LoRaWAN");  
  u8g2.sendBuffer();

  // MAX31865
  pinMode(4, OUTPUT);
  Serial.println(F("Adafruit MAX31865 PT100 Sensor Test!"));
#ifdef PT100_4W
  thermo.begin(MAX31865_4WIRE); // set to 2WIRE, 3WIRE or 4WIRE as necessary
#endif

#ifdef PT1000_3W
//  thermo.begin(MAX31865_3WIRE); // set to 2WIRE, 3WIRE or 4WIRE as necessary
#endif

  // // BME280
  // Serial.println(F("BME280 test"));

  // unsigned status;

  // // default settings
  // // status = bme.begin();
  // // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire);
  // if (!status)
  // {
  //   Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
  //   Serial.print("SensorID was: 0x");
  //   Serial.println(bme.sensorID(), 16);
  //   Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
  //   Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
  //   Serial.print("        ID of 0x60 represents a BME 280.\n");
  //   Serial.print("        ID of 0x61 represents a BME 680.\n");
  //   while (1)
  //     delay(10);
  // }

  // misura anemometro
  anemometroOn();

  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868))
  {
    Serial.println("Failed to start module");
    u8g2.clearBuffer();
    u8g2.drawStr(0, 16, "LoRaWAN modem");
    u8g2.drawStr(0, 31, "failed to start");
    u8g2.sendBuffer();
    while (1)
    {
    }
  };

  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
  Serial.print("Your end device IS is: ");
  Serial.println(END_DEVICE_ID);

  int connected = modem.joinOTAA(appEui, appKey);

  u8g2.setFont(u8g2_font_7x14_tf);

  if (!connected)
  {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    u8g2.clearBuffer();
    u8g2.drawStr(0, 16, "no LoRaWAN signal");
    u8g2.drawStr(0, 31, "are you indoor?");
    u8g2.sendBuffer();
    delay(5000);
  }
  else
  {
    isOnline = true;
    Serial.println("LoRaWAN connected");
    u8g2.clearBuffer();
    u8g2.drawStr(0, 16, "LoRaWAN connected");
    u8g2.sendBuffer();
    // Set poll interval to 60 secs.
    modem.minPollInterval(60);
    // NOTE: independent of this setting, the modem will
    // not allow sending more than one message every 2 minutes,
    // this is enforced by firmware and can not be changed.
  }
}

// --- loop function ----------------------------------------------------------
void loop()
{
  lettura(&misura);
  
  // print on OLED
  u8g2.clearBuffer();
  // u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.setFont(u8g2_font_7x14_tf);

  // BME280Temperature
  //sprintf(msg, "BME: %6.2f C", misura.BME280Temperature);
  //u8g2.drawStr(0, 32, msg);
  //sprintf(msg, "BME: %6.2f %", misura.BME280Humidity);
  //u8g2.drawStr(0, 48, msg);
  
  // LM35Temperature
  sprintf(msg, "LM35: %6.2f C", misura.LM35Temperature);
  u8g2.drawStr(0, 48, msg);
  Serial.println(msg);

  // RTDTemperature
  sprintf(msg, "Resistance: %10.6f Ohm", misura.RTDResistance);
  Serial.println(msg);   
  sprintf(msg, "RTD: %6.2f C", misura.RTDTemperature);
  u8g2.drawStr(0, 62, msg);
  Serial.println(msg);  

  u8g2.sendBuffer();

  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault)
  {
    Serial.print("Fault 0x");
    Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH)
    {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH)
    {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW)
    {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH)
    {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW)
    {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV)
    {
      Serial.println("Under/Over voltage");
    }
    thermo.clearFault();
  }
  Serial.println();

  // velocità vento
  sprintf(msg, "vento: %5ld us, %6.3f m/s\n", misura.WindPeriod, misura.WindSpeed);
  Serial.print(msg);

  // aggiorna il documento JSON
  if (lastTime + LORA_PUBLISH_PERIOD < millis())
  {
    lastTime = millis();

    encode(ttnDataPacket, misura.LM35Temperature, eLM35);
    encode(ttnDataPacket, misura.RTDTemperature, eRTD);

    for(int i=0; i<PKTSIZE; i++) {
      Serial.print(ttnDataPacket[i],HEX); Serial.print(" ");
    }
    Serial.println();

    if (isOnline)
    {
      // send the uplink to TTN
      int err;
      modem.beginPacket();
      modem.write(ttnDataPacket, PKTSIZE);

      // don't ask TTN for acknowledge after uplink
      err = modem.endPacket(false);
      if (err > 0)
      {
        Serial.println("Message sent correctly!");
      }
      else
      {
        Serial.println("Error sending message :(");
        Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
        Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
      }
    }
  }

// ritardo o modalità sleep
#ifdef _SLEEP
  LowPower.sleep(10000);
#else
  delay(10000);
#endif
}

void lettura(TMisura *m) {

  // // misura di temperatura e umidità dal sensore BME280
  //m->BME280Temperature = bme.readTemperature();
  //m->BME280Humidity = bme.readHumidity();

  // misura di temperatura dal sensore PT100
  uint16_t rtd = thermo.readRTD();
  m->RTDTemperature = thermo.temperature(RNOMINAL, RREF);
  float ratio = rtd;
  ratio /= 32768.0;
  m->RTDResistance = RREF * ratio;   

  // misura di temperatura dal LM35
  m->LM35Temperature = lm35.temperature();

  // periodo anemometro
  m->WindPeriod = anemometroPeriodo();
  // velocità del vento
  m->WindSpeed = anemometroVelocita();

}