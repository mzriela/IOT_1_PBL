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
 //an array of 10 readings(From LAB11) - Sensing and Display
 const int NUM_READING = 10;
 int samples[NUM_READING];

 // add Fan-related pin control - Fan Control
 #define POT_PIN 34
 #define LED_PIN 2
 // LED_State_configuretions(From LAB12) - Fan Control
 const int ADC_BITS = 12;
 const int PWM_FREQ = 5000;     // PWM frequency (Hz)
 const int PWM_RES = 8;         // 8 bits: duty 0 – 255

 //UART-related GPIO Pins - Events and Status Byte Lab 2 and 3
 #define TX 1
 #define RX 3

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
  float Text_Size = 2;
  //
  Adafruit_BMP280 BMP;
  float temperature = 0; 
  // 8-bit Status byte
  uint8_t value;
// Functions
 void sendReport(float average,int level, int array)
 {
  // A JsonDocument holds JSON data in memory
  JsonDocument report;
  // Add key : value pairs (Common Commands: "Add a Key and Value").
  report["device"] = "";    // text
  report["average"] = average;   // number with decimals
  report["status"] = array;
 }
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
 // SetRoomStatus() - Set the room status on the OLED based on the average temperature and the average temperature
 void SetRoomStatus(){

  float average = getAverage();
  // Set the average temp on the OLED
  display.setCursor(0,0);
   display.println("Avg Temp:");
   display.print(average);
   display.print("C"); 
  // if conditions
  if(average < 20){
   display.println("    RoomStatus: Cold");
  }
  else{
   display.println("    RoomStatus: Hot");
  }
 }
 // partPWM() - convert potieminometer value into duty value for the LED
 void partPWM()
 {
  int potValue = analogRead(POT_PIN);
 // Brightness follows the pot (Common Commands: "Scale a Value")
 // "Room is occupied"
 int duty = map(potValue, 0, 4095, 0, 255);     // 0 – 4095 → 0 – 255
 if(potValue<1024){
 ledcWrite(LED_PIN, 64);  // set the LED duty-Led level 1
 }
  else if(potValue<2048 && potValue>1025){
 ledcWrite(LED_PIN, 128); // set the LED duty-Led level 2
 }
 else{ 
 ledcWrite(LED_PIN, 255); // set the LED duty-Led level 3
 }
 Serial.print("duty = ");  Serial.print(potValue);
 Serial.print("   duty cycle = ");  Serial.print(duty * 100 / 255);  Serial.println(" %");
 delay(100);
 }
 void StatusByte(){
  byte value = 0;

  value |= 

 }

void setup() {
 Serial.begin(115200); 
 //Start the BMP280 and SPI comm
 Wire.begin(); 
 BMP.begin(0x77);
 display.begin(SSD1306_SWITCHCAPVCC); 
 pinMode(33,OUTPUT);
  // ADC settings
  analogReadResolution(12);            // 12 bits: values 0 – 4095
  analogSetAttenuation(ADC_11db);      // measure the full 0 – 3.3 V range
  // PWM settings
  ledcAttach(LED_PIN, PWM_FREQ,PWM_RES);
 // Clear Display 
 display.clearDisplay(); 
 // Set Text Size 
 display.setTextSize(Text_Size);
 // Set Text Color 
 display.setTextColor(SSD1306_WHITE); 
 // Update Display 
 display.display();
 }
void loop(){
  getTemperature();
  float average = getAverage();
 // Set room status on OLED
 SetRoomStatus();
 //temporary
 printSamples();
 //partPWM();
 partPWM();

 // Update the display 
 display.display();
 Serial.println(average); 

}
