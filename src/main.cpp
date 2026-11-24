// IoT PBL - Smart Study Room Station
// By: Jason and Julia 

// ==================================================
//                    LIBRARIES
// ==================================================
#include <Arduino.h>
// From Lab 9
#include <SPI.h>
#include <MFRC522.h>
// From Lab 6
#include <SD.h>

// ==================================================
//                  PIN DEFINITIONS
// ==================================================
// Shared SPI bus: SCK: GPIO 18, MOSI: GPIO 23, MISO: GPIO 19
#define RFID_SS 5
#define RFID_RST 27

#define GREEN_LED 25 // Green LED: ON = room occupied, OFF = room free
#define RED_LED 26 // Red LED: flashes when an unknown card is tapped


#define SD_CS 13
#define OLED_CS 4


// ==================================================
//                     OBJECTS
// ==================================================
MFRC522 rfid(RFID_SS, RFID_RST);

File dataFile; // File object for SD card

// ==================================================
//                 AUTHORIZED CARDS
// ==================================================
// Lists the cards that are allowed (white cards)
String authorizedUIDs[] = {"D0 68 C8 5F", "53 52 A4 56"}; // Add authorized UIDs here
int numAuthorized = 2;

// Stores the room state. "False" is free & "True" is Occupied
// Starts as free, but will be set to occupied when the first card is scanned
bool occupied = false; 

// ==================================================
//               TIMER AND SCAN VARIABLES
// ==================================================
unsigned long indicatorStart = 0;
unsigned long indicatorTime = 0;
bool indicatorOn = false;

String lastUID = ""; // Store the last scanned UID to prevent repeated scans
unsigned long lastScanTime = 0; // Store the time of the last scan

String occupiedBy = "";   // UID of the card that checked in


// ==================================================
//                     FUNCTIONS
// ==================================================

// ---------- Reading the Card ----------
// getUID() turns the card's UID bytes into text
String getUID() { 
  String uid = ""; 
    // Go thru every byte of the UID (4 bytes)
    for (byte i = 0; i < rfid.uid.size; i++) {
      // If the byte is less than 0x20, add a "0" so it always has 2 digits (05 not 5)
      if (rfid.uid.uidByte[i] < 0x10) {
        uid = uid + "0";
    }
   
    uid = uid + String(rfid.uid.uidByte[i], HEX);
      if (i < rfid.uid.size - 1) {
        uid = uid + " ";
    }
  }

    uid.toUpperCase(); // Makes the letters capitals so it matches the list
    return uid;
}

// ---------- Checking the Card ----------
// isAuthorized() checks if the card's UID is in the list of authorized UIDs
  bool isAuthorized(String uid) {
    for (int i = 0; i < numAuthorized; i++) { // Compares the scanned UID with every UID in the list
      if (uid == authorizedUIDs[i]) {
        return true;
      }
    }
    return false; // Checked the whole list with no match, so the card is not authorized
  }

  // ---------- Indicator Timer ----------
  void startIndicatorTimer(unsigned long duration) {
    indicatorStart = millis();
    indicatorTime = duration; 
    indicatorOn = true;
  }

 // ---------- Event: Unknown Card (Denied) ----------
// Unknown card: flash the red LED (the room state doesn't change)
    void onAccessDenied() 
    { 
      Serial.println("ACCESS DENIED");
      for (int i = 0; i < 3; i++) { // Flash the red LED 3 times
        digitalWrite(RED_LED, HIGH); // Red LED on
        delay(200);
        digitalWrite(RED_LED, LOW); // Red LED off
        delay(200);
      }
    }

// ---------- Event: Authorized Card (Granted) ----------
// Authorized card: toggle the room between Free and Occupied
  void onAccessGranted(String uid) {
    if (occupied == false) {
    // Room is free: this card checks in
    occupied = true;
    occupiedBy = uid; // Remember which card checked in
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("Room is now Occupied");
  }
  else if (uid == occupiedBy) {
    // Same card that checked in: check out
    occupied = false;
    occupiedBy = "";
    digitalWrite(GREEN_LED, LOW);
    Serial.println("Room is now Free");
  }
  else {
    // A different authorized card: room is in use, so refuse it
    Serial.println("Room is in use by another card");
    onAccessDenied();
  }
}

// ---------- Event: Timer Expired ----------
// Timer expired: turn off the red LED only (the green LED shows the room state)
    void onTimerExpired() {
      digitalWrite(RED_LED, LOW);
      indicatorOn = false;
      Serial.println("Ready");
    }

// ---------- Event: Card Scanned ----------
// Runs every time a card is read: ignores repeats, then checks the list
    void onCardScanned(String uid) {
      // Same card as last time AND less than 2 seconds ago = same tap, so ignored
      if (uid == lastUID && millis() - lastScanTime < 2000) {
        Serial.println("Repeat ignored");
        return; // Ignore repeated scans within 2 seconds
      }

      // Remember this card and the time for the next check
      lastUID = uid;
      lastScanTime = millis();

      Serial.print("Card UID: ");
      Serial.println(uid);

      // On the list = toggle the room, not on the list = flash red
      if (isAuthorized(uid) == true) {
        onAccessGranted(uid);
      }
      else {
        onAccessDenied();
      }
    }

// ---------- SD Card Functions ----------
// ---------- Write one event to the CSV file ----------
void logEvent(String event, String uid) {
  unsigned long currentTime = millis() / 1000; // Get the current time in seconds

  dataFile = SD.open("log.csv", FILE_WRITE); // Open the file for writing
  if (dataFile) {
    dataFile.print(currentTime);
    dataFile.print(",");
    dataFile.print(event);
    dataFile.print(",");
    dataFile.println(uid);
    dataFile.close(); // Close the file
  } else {
    Serial.println("Error opening log.csv");
  }

  Serial.print(currentTime);
  Serial.print(" s, ");
  Serial.print(event);
  Serial.print(", ");
  Serial.println(uid);
}


// ==================================================
//                      SETUP
// ==================================================
void setup() {
  Serial.begin(115200);
  Serial.println("Program Started");

  // ---------- RFID code ----------
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  delay(10); // Show details of PCD - MFRC522 Card Reader details
  rfid.PCD_DumpVersionToSerial();
  Serial.println("Room is FREE. Tap a card...");

  // ---------- LED code ----------
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // ---------- SD card code ----------
  pinMode(RFID_SS, OUTPUT);
  pinMode(SD_CS, OUTPUT);

  Serial.println("Initializing SD Card...");
  if (SD.begin(SD_CS)) {
    Serial.println("SD Card Found");
  }
  else {
    Serial.println("SD Card Not Found");
  }

}

// ==================================================
//                       LOOP
// ==================================================
void loop() {
  // ---------- Card detection ----------
  if (rfid.PICC_IsNewCardPresent() == true && rfid.PICC_ReadCardSerial() == true) {
    String uid = getUID();
    onCardScanned(uid);
    rfid.PICC_HaltA(); // Stop reading (puts the card to sleep so it is not read again and again)
  }
}



