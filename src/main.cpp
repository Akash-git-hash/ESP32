
#include "IO_setup.h"
#include <VL53L0X.h>

#define NUM_SENSORS 2

VL53L0X ToF[NUM_SENSORS];

// GPIO's - XSHUT PINS
const uint8_t XSHUT[NUM_SENSORS] = {IO1, IO2}; 
// I2C Sensor Address
const uint8_t ADDR[NUM_SENSORS] = {0x30, 0X31}; 
// Array to Stor Height
uint16_t height[NUM_SENSORS] = {0};

void Heartbeat();
void ToF_init();
void read_ToF();

void setup() {

  // Heartbeat LED Setup
  pinMode(BUILTIN_LED_PIN, OUTPUT);

  //I2C Setup
  Wire.begin(I2C_SDA, I2C_SCL);
  ToF_init();
}

void loop() {
  
  Heartbeat(); 
  delayMicroseconds(1000);

  if(Serial.available()){

    read_ToF();
    for(int i=0;i<NUM_SENSORS;i++){
      Serial.printf("Sensor %d: %d mm\n", i, height[i]);
    }
  }

  delay(10);

}

void Heartbeat()
{
  digitalWrite(BUILTIN_LED_PIN, HIGH);
  delay(500);
  digitalWrite(BUILTIN_LED_PIN, LOW);
  delay(500);
}

void ToF_init()
{
  Serial.begin(115200);

  // Turn off all sensors
  for(int i=0; i<NUM_SENSORS; i++){
    pinMode(XSHUT[i], OUTPUT);
    digitalWrite(XSHUT[i], LOW);
  }
  delay(10);

  // Initialize each sensor one by one
  for(int i=0;i<NUM_SENSORS;i++){
    digitalWrite(XSHUT[i], HIGH);
    delay(10);

    if(!ToF[i].init()){
      Serial.printf("Failed init...");
      return;
    }

    ToF[i].setAddress(ADDR[i]);
    ToF[i].setTimeout(500);
    Serial.printf("Init Success: Sensor %d Address 0x%x\n", i, ADDR[i]);
  }
}

void read_ToF()
{
  for(int i=0;i<NUM_SENSORS;i++){
    height[i] = ToF[i].readRangeSingleMillimeters();
    if(ToF[i].timeoutOccurred()){
      Serial.printf("Sensor %d timeout\n", i);
      height[i] = 0;
    }
  }
}