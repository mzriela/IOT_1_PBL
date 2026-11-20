// IoT PBL - Smart Study Room Station
// By: Jason and Julia 

// ---------- Libraries ----------
#include <Arduino.h>
// From Lab 9
#include <SPI.h>
#include <MFRC522.h>
// From Lab 6
#include <Wire.h> 
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// From Lab 7
#include <WiFi.h>
#include <WebServer.h>


// ---------- Global Variables ----------
// add pins

// ---------- Objects ----------
MFRC522 rfid(RFID_SS, RFID_RST);

String authorizedUIDs[] = {"D0 68 C8 5F", "53 52 A4 56"}
const int numAuthorized = 2;

// ---------- Functions ---------- 
// getUID() turns the card's UID bytes into text

String uid = ""; 
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid = uid + "0";
    }
   
   uid = uid + String(rfid.uid.uidByte[i], HEX);
     if (i < rfid.uid.size - 1) {
      uid = uid + " ";
    }
  }










