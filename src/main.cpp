// IoT PBL - Smart Study Room Station
// By: Jason and Julia 

// ---------- Libraries ----------
#include <Arduino.h>
// From Lab 9
#include <SPI.h>
#include <MFRC522.h>

// ---------- Global Variables ----------
#define RFID_SS ___
#define RFID_RST ___

#define GREEN_LED ___
#define RED_LED ___

// ---------- Objects ----------
MFRC522 rfid(RFID_SS, RFID_RST);

String authorizedUIDs[] = {"D0 68 C8 5F", "53 52 A4 56"}
const int numAuthorized = 2;

// ---------- Functions ---------- 
// getUID() turns the card's UID bytes into text
String getUID() { 
  String uid = ""; 
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[] < 0x10) {
        uid = uid + "0";
    }
   
    uid = uid + String(rfid.uid.uidByte[], HEX);
      if (i < rfid.uid.size - 1) {
        uid = uid + " ";
    }
  }

    uid.toUpperCase();
    return uid;
}


void onAccessGranted()
{
  Serial.println("ACCESS GRANTED");
  allIndicatorsOff();
  digitalWrite(GREEN_LED, HIGH);
  startIndicatorTimer(2000);
}


void onAccessDenied()
{
  Serial.println("ACCESS DENIED");
  allIndicatorsOff();
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);
  startIndicatorTimer(1000);
}









