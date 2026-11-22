// IoT PBL - Smart Study Room Station
// By: Jason and Julia 

// ---------- Libraries ----------
#include <Arduino.h>
// From Lab 9
#include <SPI.h>
#include <MFRC522.h>

// ---------- Global Variables ----------
// Shared SPI bus: SCK: GPIO 18, MOSI: GPIO 23, MISO: GPIO 19
#define RFID_SS 5
#define RFID_RST 27

#define GREEN_LED 25
#define RED_LED 26

// ---------- Objects ----------
MFRC522 rfid(RFID_SS, RFID_RST);

String authorizedUIDs[] = {"D0 68 C8 5F", "53 52 A4 56"}; // Add authorized UIDs here
int numAuthorized = 2;

// Stores the room state. "False" is free & "True" is Occupied
// Starts as free, but will be set to occupied when the first card is scanned
bool occupied = false; 

unsigned long indicatorStart = 0;
unsigned long indicatorTime = 0;
bool indicatorOn = false;

String lastUID = ""; // Store the last scanned UID to prevent repeated scans
unsigned long lastScanTime = 0; // Store the time of the last scan


// ---------- Functions ---------- 
// getUID() turns the card's UID bytes into text
String getUID() { 
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

    uid.toUpperCase();
    return uid;
}

  bool isAuthorized(String uid) {
    for (int i = 0; i < numAuthorized; i++) {
      if (uid == authorizedUIDs[i]) {
        return true;
      }
    }
    return false;
  }

  void startIndicatorTimer(unsigned long duration) {
    indicatorStart = millis();
    indicatorTime = duration;
    indicatorOn = true;
  }

// Authorized card: toggle the room between Free and Occupied
  void onAccessGranted() {
    occupied = !occupied; // Toggle the room state
    digitalWrite(GREEN_LED, occupied);

    if (occupied == true)
    {
      Serial.println("Room is now Occupied");
    }
    else 
    {
      Serial.println("Room is now Free");
    }
  }
// Unknown card: flash the red LED (the room state doesn't change)
    void onAccessDenied() 
    { 
      Serial.println("ACCESS DENIED");
      digitalWrite(RED_LED, HIGH);
      startIndicatorTimer(500); // Flash the red LED for 0.5 second
    }

// Timer expired: turn off the red LED only (the green LED shows the room state)
    void onTimerExpired() {
      digitalWrite(RED_LED, LOW);
      indicatorOn = false;
      Serial.println("Ready");
    }

    void onCardScanned(String uid) {
      if (uid == lastUID && millis() - lastScanTime < 2000) {
        Serial.println("Repeat ignored");
        return; // Ignore repeated scans within 2 seconds
      }

      lastUID = uid;
      lastScanTime = millis();

      Serial.print("Card UID: ");
      Serial.println(uid);

      if (isAuthorized(uid) == true) {
        onAccessGranted();
      }
      else {
        onAccessDenied();
      }
    }

void setup() {
  Serial.begin(115200);
  Serial.println("Program Started");

  // RFID code
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  delay(10); // Show details of PCD - MFRC522 Card Reader details
  rfid.PCD_DumpVersionToSerial();
  Serial.println("Room is FREE. Tap a card...");

  // LED code
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

}


void loop() {
  // Card detection
  if (rfid.PICC_IsNewCardPresent() == true && rfid.PICC_ReadCardSerial() == true) {
    String uid = getUID();
    onCardScanned(uid);
    rfid.PICC_HaltA(); // Stop reading
  }

  // Timer for indicator LED
  if (indicatorOn == true && millis() - indicatorStart >= indicatorTime) {
    onTimerExpired();
  }
}



