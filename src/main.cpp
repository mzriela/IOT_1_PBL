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
#include <Fan.h>
// ---------- Pin Numbers ----------
 // add OLED Display Settings (From Lab6) - Sensing and Display
 #define SCREEN_WIDTH 128 
 #define SCREEN_HEIGHT 64 
 // add OLED GPIO pins (From Lab6) - Sensing and Display
 #define OLED_DIN 23
 #define OLED_CLK 18 
 #define OLED_CS 4
 #define OLED_DC 16 
 #define OLED_RES 17 
 // add BMP280 GPIO pins (From Lab6) - Sensing and Display
 #define BMP280_SDA 21
 #define BMP280_SCL 22
 // add Fan-related pin control
 #define FAN_PIN 33
 #define POT_PIN 34

 //an array of 10 readings(From LAB11)
 const int NUM_READING = 10;
 int samples[NUM_READING];
 #define FAN_CH    0            // PWM channel for the LED
 const int ADC_BITS = 12;
 const int PWM_FREQ = 5000;     // PWM frequency (Hz)
 const int PWM_RES = 8;         // 8 bits: duty 0 – 255
 // ---------- Global Variables ---------- 
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
// Part VII – PWM: LED brightness follows the potentiometer(From L12)
void partPWM()
{
  int potValue = analogRead(POT_PIN);

// Brightness follows the pot (Common Commands: "Scale a Value")
int duty = map(potValue, 0, 4095, 0, 255);     // 0 – 4095 → 0 – 255
ledcWrite(FAN_CH, duty);                        // set the LED duty

Serial.print("duty = ");  Serial.print(duty);
Serial.print("   duty cycle = ");  Serial.print(duty * 100 / 255);  Serial.println(" %");
delay(100);

}

void setup() {
 Serial.begin(115200); 
 Wire.begin(); 
 BMP.begin(0x77);
 display.begin(SSD1306_SWITCHCAPVCC); 
 digitalWrite(33,OUTPUT);
 //
  analogReadResolution(12);  
  analogSetAttenuation(ADC_11db);  
    // PWM settings


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
 partPWM();
 

  // set location where text appears
 display.setCursor(0,0);
 // Update the display 
 display.display(); 

  // Wait one second 
  delay(1000); 
}
