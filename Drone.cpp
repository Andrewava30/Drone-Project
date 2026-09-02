#include <WiFi.h>
#include <esp_now.h>
#include <MPU6050_tockn.h>
#include <Wire.h>

const int SDA = 15;
const int SCL = 18;

MPU6050 mpu6050(Wire);

float ax, ay, az;
float gx, gy, gz;

unsigned long timer = 0;

// Pins



/* Data used for stable motion */
// Want to Turn all Controller Inputs into Throttle, Roll, Pitch, and Yaw, then combine them in calculating each motor
int baseThrottle = 50;  // 50 = hover
int baseRoll = 0;
int basePitch = 0;
int baseYaw = 0;


/* References:
    Throttle = Up and down Motion (Right Y)
        Down = -100
        Up = 100
    Roll = Left and Right Motion (Left X)
        Left = -100
        Right = 100
    Pitch = Foward and Back Motion (Left Y)
        Backward = -100
        Foward = 100
    Yaw = Rotation Left and Right (Right X)
        Left = -100
        Right = 100
*/

struct ControllerData
{
  /* data */
  // All Joystick Values are from -100 to 100
  int16_t LeftJoystickX;  // Left/Right Motion
  int16_t LeftJoystickY;  // Foward/Back Motion
  int16_t RightJoystickX; // Left/Right Turing
  int16_t RightJoystickY; // Up/Down Motion

  //bool LeftButton;    // Unused, Button on left joystick
  //bool RightButton;   // Unused, Button on Right joystick

  uint32_t PacketNumber;  // Used to keep track if the drone didn't recieve some packets
};

ControllerData data;        // data holds the Controller data


// Motors
int Motor1;     // Front Left Motor (CW)
int Motor2;     // Front Right Motor (CCW)
int Motor3;     // Back Left Motor (CCW)
int Motor4;     // Back Right Motor (CW)

  
void MoveLeft(int LeftjoystickX) {

};
void MoveRight(int LeftjoystickX) {
  // Use function for any part that might use tilting and need to be adjusted with the gyroscope
};


/**
 * @brief Used when losing signal with controller to land slowly till it reaches the ground or reconnects with controller
 */
void emergencyland() {
  // Used when losing connection to controller 
};

/**
 * @brief Adjusts values of motors based off gyroscope to keep motion stable,
 * @attention This Function will be a pain to optmize and get right
 * @param motor What motor is being adjusted
 * @param throttle Current Throttle Value
 * @param roll Current Roll Value
 * @param yaw Current Yaw Value
 * @return Value to adjust the specified motor by
 */
int gyroscopeAdjustments(int motor, int throttle, int roll, int pitch, int yaw) {

}

void setup() {
  // Runs on startup
  Serial.begin(115200);

  Wire.begin(SDA, SCL);

  mpu6050.begin();

  mpu6050.calcGyroOffsets(true);

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
  Serial.print("GyroScope Data: AX: ")
  Serial.print(ax);
  Serial.print(",");

  Serial.print(ay);
  Serial.print(",");

  Serial.print(az);
  Serial.print(",");

  Serial.print(gx);
  Serial.print(",");

  Serial.print(gy);
  Serial.print(",");

  Serial.println(gz);


  int throttle = baseThrottle + data.RightJoystickY / 2; // 0 = up, 4095 = down
  int pitch = 0;
  int yaw = 0;
  int 

  // Want to have all of them end with something like
  // int motor1 = throttle + pitchCorrection + rollCorrection - yawCorrection + gyroscopeAdjustments


  if (millis() - lastPacketTime > 200) {
  // CONNECTION LOST
  // Immediately put drone into failsafe
    emergencyland();
  };

  delay(20); // 50Hz update
}