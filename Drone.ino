#include <WiFi.h>
#include <esp_now.h>
#include <MPU6050_tockn.h>
#include <Wire.h>


MPU6050 mpu6050(Wire);

float ax, ay, az;
float gx, gy, gz;

unsigned long timer = 0;

// Pins

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

  
void MoveLeft(int LeftjoystickX) {

};
void MoveRight(int LeftjoystickX) {
  // Use function for any part that might use tilting and need to be adjusted with the gyroscope
};


void emergencyland() {
  // Used when losing connection to controller 
};

void adjustMotors() {
};

// callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));

  int Roll = data.LeftJoystickX;
  int Pitch = data.LeftJoystickY;
  int Yaw = data.RightJoystickX;
  int throttle = data.RightJoystickY;

  /*
  Used to test what data is being recieved from controller
  */

  Serial.print("Throttle: ");
  Serial.print(data.RightJoystickY);
  Serial.print(" Pitch: ");
  Serial.print(data.LeftJoystickY);
  Serial.print(" Roll: ");
  Serial.print(data.LeftJoystickX);
  Serial.print(" Yaw: ");
  Serial.println(data.RightJoystickX);
  
};

void setup() {
  // Runs on startup
  Serial.begin(115200);


  Wire.begin(SDA, SCL);

  mpu6050.begin();

  mpu6050.calcGyroOffsets(true);

  WiFi.mode(WIFI_STA);
  
  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);


  delay(1000);
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
  
  /*
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
  */
  
/*
  if (millis() - lastPacketTime > 200) {
  // CONNECTION LOST
  // Immediately put drone into failsafe
  };
  */

  delay(10); // 50Hz update
}