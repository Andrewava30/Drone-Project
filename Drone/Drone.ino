#include <WiFi.h>
#include <esp_now.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <ESP32Servo.h>

Servo ESC1;
Servo ESC2;
Servo ESC3;
Servo ESC4;

/* 1000 = Stopped, 2000 = Full Speed */
const int minPulse = 1000;  // Minimum Motor pulse (0% power)
const int maxPulse = 2000;  // Maximum Motor pulse (100% power)

// Gyroscope
MPU6050 mpu6050(Wire);

float ax, ay, az;
float gx, gy, gz;

unsigned long lastPacketTime = 0;   // Time that the last controller packet was recieved
bool failSafeMode = false;    // Is the drone in a failsafe mode (not recieving packets from controller)

// Keeps track of the motor speeds
int motor1 = 0;   // Battery Side Left Motor (SPIN?)
int motor2 = 0;   // Battery Side Right Motor (SPIN?)
int motor3 = 0;   // ESP32 Right Motor (SPIN?)
int motor4 = 0;   // ESP32 Left Motor (SPIN?)

// Pins
const int escPin1 = 5;       // ESP32-S3 GPIO pin connected to ESC signal
const int escPin2 = 6;
const int escPin3 = 7;
const int escPin4 = 8; 

// NOTE Looking at this from the ESP32 Side, Gyroscope is Right side

struct ControllerData
{
  /* data */
  // All Joystick Values are from -100 to 100
  int16_t LeftJoystickX;  // Left/Right Motion (Roll)
  int16_t LeftJoystickY;  // Foward/Back Motion (Pitch)
  int16_t RightJoystickX; // Left/Right Turing (Yaw)
  int16_t RightJoystickY; // Up/Down Motion (Throttle)

  //bool LeftButton;    // Unused, Button on left joystick
  //bool RightButton;   // Unused, Button on Right joystick

  uint32_t PacketNumber;  // Used to keep track if the drone didn't recieve some packets
};

ControllerData data;        // data holds the Controller data

int Roll = 0;   // Left X, Left/Right
int Pitch = 0;  // Left Y, Foward/Back
int Yaw = 0;    // Right X, Turn Left/Right
int Throttle = 0; // Right Y, Up/Down
int PacketNum = 0;  // Used to keep track of packets


int baseSpeed = 1100;  // Electrical Frequency base for motors (should be speed to keep drone stable in air)


/**
 * @brief Checks if Failsafe should activate or not if packets not recieved for 1 second
 */
void checkFailsafe() {
  // Check if more than 1 second has passed since last packet was recieved
  if (millis() - lastPacketTime > 1000) {
    if (!failSafeMode) {
      failSafeMode = true;

      // Connection has been lost
      Serial.println("Connec tion lost");
    }
  }
  else {
    if (failSafeMode) {
      failSafeMode = false;

      // Packets have returned
      Serial.println("Connection restored");
    }
  }
}

void emergencyland() {
  // Used when losing connection to controller 
   ESC1.writeMicroseconds(minPulse);
   ESC2.writeMicroseconds(minPulse);
   ESC3.writeMicroseconds(minPulse);
   ESC4.writeMicroseconds(minPulse);
};
/**
 * @brief Adjusts the motor speeds based on the GyroScope
 * @param motor Motor # (1-4) that this calculates for
 * @param threshold the threshold needed to change motors
 * @param GyroX GyroScope X value
 * @param GyroY GyroScope Y value
 * @param GyroZ GyroScope Z value
 */
void adjustMotorsGyro(int motor, float threshold, float GyroX, float GyroY, float GyroZ) {
  if abs(GyroX)
};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  // Update Values from Controller
  Roll = data.LeftJoystickX;
  Pitch = data.LeftJoystickY;
  Yaw = data.RightJoystickX;
  Throttle = data.RightJoystickY;
  PacketNum = data.PacketNumber;
  lastPacketTime = millis();    // Get the time that the packet was recieved
  
};

void setup() {
  // Runs on startup
  Serial.begin(115200);

  Wire.begin(SDA, SCL);

  mpu6050.begin();

  mpu6050.calcGyroOffsets(true);

  WiFi.mode(WIFI_STA);

  // Get the ESP32's MAC address
  Serial.println();
  Serial.print("Drone MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  // --- ESC ARMING SEQUENCE ---

  // Give them a starting frequency
  ESC1.setPeriodHertz(50);      // Standard servo frequency for ESCs
  ESC2.setPeriodHertz(50);      // Standard servo frequency for ESCs
  ESC3.setPeriodHertz(50);      // Standard servo frequency for ESCs
  ESC4.setPeriodHertz(50);      // Standard servo frequency for ESCs

  // Attach the pins
  ESC1.attach(escPin1, minPulse, maxPulse); 
  ESC2.attach(escPin2, minPulse, maxPulse); 
  ESC3.attach(escPin3, minPulse, maxPulse); 
  ESC4.attach(escPin4, minPulse, maxPulse); 

  // Set min throttle to arm them
  ESC1.writeMicroseconds(minPulse);
  ESC2.writeMicroseconds(minPulse);
  ESC3.writeMicroseconds(minPulse);
  ESC4.writeMicroseconds(minPulse);

  delay(3000);

  Serial.println("ESC armed");
}

void loop() {
  // Runs Continously
  mpu6050.update();

  // Accelerometer
  ax = mpu6050.getAccX();
  ay = mpu6050.getAccY();
  az = mpu6050.getAccZ();

  // Gyroscope
  gx = mpu6050.getGyroX();
  gy = mpu6050.getGyroY();
  gz = mpu6050.getGyroZ();

  // Send clean CSV data
  
  Serial.print("Accel X: ");
  Serial.print(ax);
  Serial.print(", ");

  Serial.print("Accel Y: ");
  Serial.print(ay);
  Serial.print(", ");

  Serial.print("Accel Z: ");
  Serial.print(az);
  Serial.print(", ");

  Serial.print("Gyro X: ");
  Serial.print(gx);
  Serial.print(", ");

  Serial.print("Gyro Y: ");
  Serial.print(gy);
  Serial.print(", ");

  Serial.print("Gyro Z: ");
  Serial.println(gz);
  
  
  //checkFailsafe();

  if (failSafeMode) {
    // Run Failsafe

    // FIXME 
    // Wait in Air for ~5-10 seconds
    // Slowly Decend down, using gyroscrope to adjust for wind
    // Once Gyroscope detects no more downward acceleration, slow down motors to a stop

  }
  else {

  /*
  Print Values from Controller
  
  Serial.print("Throttle: ");
  Serial.print(Throttle);
  Serial.print(" Pitch: ");
  Serial.print(Pitch);
  Serial.print(" Roll: ");
  Serial.print(Roll);
  Serial.print(" Yaw: ");
  Serial.print(Yaw);
  Serial.print(" Packet #: ");
  Serial.println(PacketNum);
  */

  // Calulate Speeds of each motor (in frequency)
  motor1 = constrain(baseSpeed + Throttle + Roll, minPulse, maxPulse);
  motor2 = constrain(baseSpeed + Throttle + Roll, minPulse, maxPulse);
  motor3 = constrain(baseSpeed + Throttle - Roll, minPulse, maxPulse);
  motor4 = constrain(baseSpeed + Throttle - Roll, minPulse, maxPulse);


  // Send frquency to each ESC to control motors
  ESC1.writeMicroseconds(motor1);
  ESC2.writeMicroseconds(motor2);
  ESC3.writeMicroseconds(motor3);
  ESC4.writeMicroseconds(motor4);
  }

  delay(10); // 10Hz update
}