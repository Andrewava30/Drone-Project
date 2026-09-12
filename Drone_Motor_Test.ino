#include <WiFi.h>
#include <ESP32Servo.h>

Servo myESC; 

const int escPin = 15;       // ESP32-S3 GPIO pin connected to ESC signal
const int minPulse = 1000;   // Minimum throttle pulse (microseconds)
const int maxPulse = 2000;   // Maximum throttle pulse (microseconds)




// Available Pins


// 1000 = Stopped
// 2000 = Full Speed


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);


  myESC.setPeriodHertz(50);      // Standard servo frequency for ESCs
  myESC.attach(escPin, minPulse, maxPulse); 

  // --- ESC ARMING SEQUENCE ---
  // Most ESCs need to see a "zero throttle" signal at startup to arm safely
  myESC.writeMicroseconds(minPulse); 
  delay(3000);                   // Wait 3 seconds for the ESC to arm (listen for beeps)

}

void loop() {
  // put your main code here, to run repeatedly:
  // 1. Run the motor for 5 seconds
  // Change 1200 to a higher value (up to 2000) for more speed
  myESC.writeMicroseconds(1200); 
  delay(5000); 

  // 2. Stop the motor for 5 seconds
  myESC.writeMicroseconds(minPulse); 
  delay(5000); 

}
