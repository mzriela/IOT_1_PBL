// IoT PBL - Smart Study Room Station
// By: Jason and Julia 

// ---------- Libraries ----------
  // Libraries from Lab 6
 #include <Arduino.h>
 #include <Wire.h>
 #include <SPI.h> 
 #include <Adafruit_Sensor.h> 
 #include <Adafruit_GFX.h> 
 #include <Adafruit_SSD1306.h> 
 #include <Adafruit_BMP280.h> 
 // Libraries from Lab 11
 #include <ArduinoJson.h>




// ---------- Global Variables ----------
 // add OLED GPIO pins (From Lab6)
 #define SCREEN_WIDTH 128 
 #define SCREEN_HEIGHT 64 
 #define OLED_DIN 23
 #define OLED_CLK 18 
 #define OLED_CS 4
 #define OLED_DC 16 
 #define OLED_RES 17 
 // add BMP280 GPIO pins (From Lab6)
 #define BMP280_SDA 21
 #define BMP280_SCL 22
 //an array of 10 readings(From LAB11)
 const int NUM_READING = 10;
 int samples[NUM_READING];
 // ---------- objects ---------- 
 Adafruit_SSD1306 display( 

  SCREEN_WIDTH, 

  SCREEN_HEIGHT, 

  OLED_DIN, 

  OLED_CLK, 

  OLED_DC, 

  OLED_RES, 

  OLED_CS 
); 
void sendReport(float average,int level)
{
  // A JsonDocument holds JSON data in memory
  // (Common Commands: "Create a JSON Document").
  JsonDocument report;

  // Add key : value pairs (Common Commands: "Add a Key and Value").
  report["device"] = "";    // text
  report["average"] = average;         // number with decimals
}
  Adafruit_BMP280 BMP; 

float temperature = 0 ; 
// Functions
  // Function that collects and stores BMP280's readings into an array. (Max 10 readings)
  void getTemperature(){
   for (int i = 0; i < NUM_READING; i++)
   {
    samples[i] = BMP.readTemperature();
    delay(500);
   }
  }
  // printSamples() – print every element of the array, one per line.
void printSamples()
{
  for (int i = 0; i < NUM_READING; i++)
  {
    Serial.print("samples[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(samples[i]);
  }
}
// getAverage() – add up every element, then divide by the number of elements.
float getAverage()
{
  long sum = 0;                        // the total starts at 0
  for (int i = 0; i < NUM_READING; i++)
  {
    sum = sum + samples[i];          // add element i to the total
  }
  return (float)sum / NUM_READING; // (return) keeps the decimals
 
 }
void SetRoomStatus(){

  float average = getAverage();
  if(average < 20){
    display.println("Room Status: Cold");
  }
  else{
    display.println("Room Status:Hot");
  }
}
void setup() {
 Serial.begin(115200); 
 Wire.begin(); 
 BMP.begin(0x77);
 display.begin(SSD1306_SWITCHCAPVCC); 

// Clear Display 

display.clearDisplay(); 

// Set Text Size 

display.setTextSize(2);

// Set Text Color 

display.setTextColor(SSD1306_WHITE); 

// Update Display 

display.display(); 

 
}
void loop(){
  getTemperature();
 float average = getAverage();
 // Set the average temp on the OLED
 display.println("Avg Temp:");
 display.print(average);
 display.print("C"); 
 // Set room status on OLED
 SetRoomStatus();
 //temporary
 printSamples();

 

  // set location where text appears
 display.setCursor(0,0);
 // Update the display 
 display.display(); 

  // Wait one second 
  delay(1000); 
}
