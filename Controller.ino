#include <WiFi.h>
#include <esp_now.h>

// Pins
const int LEFT_X_PIN = 0; // Gray
const int LEFT_Y_PIN = 1; // Purple
//const int LEFT_BUTTON_PIN = 0;
const int RIGHT_X_PIN = 2;  // Gray
const int RIGHT_Y_PIN = 3;  // Purple
//const int RIGHT_BUTTON_PIN = 0;

const int BUZZER_PIN = 5;

/*
Wiring:
Y axis = Gray (0,2)
X axis = Purple (1,3)
*/

/*
When at rest (5V):
Left X: 3400 +- 100 (650 +- 100)
Left Y: 3350 +- 100  (750 += 100)
Right X: 3650 +- 100
Right Y: 3400 +- 100

Max = 4095
Min = 0

Need to Scale to -100 to 100 for each, or to specific throttle and other positions
*/

/*
3.3V Joystick charge ratings
Left X Center: 1810 (2285 base)
Left Y Center : 1870 (2225 base)
Right X Center : 2730
Right Y Center : 2260
*/

// Data used for Joystick Scaling and Calulations
int LeftXCenter = 2250;     //0-4095
int LeftYCenter = 2200;     //0-4095
int RightXCenter = 2700;    //0-4095 
int RightYCenter = 2250;    //0-4095
int Deadzone = 10;          // Amount of deadzone (-100-100) that if joystick values are between then no motion occurs
const int Max = 4000;       // Max Allowed Value (cause sometimes can't reach 4095)
const int Min = 100;        // Min Allowed Value (cause sometimes can't reach 0)


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

uint32_t packetNumber = 0;

// Replace this with the MAC address of the receiving ESP32
uint8_t receiverAddress[] = {
  0x44, 0x1B, 0xF6, 0xFE, 0x66, 0xE4
};


// Callback when data is sent (Not needed but good to have)
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Plays a note for frequency and duration
void playNote(int frequency, int duration) {
  ledcAttach(BUZZER_PIN, frequency, 8);
  ledcWriteTone(BUZZER_PIN, frequency);

  delay(duration);

  ledcWriteTone(BUZZER_PIN, 0);
  delay(30);
}

// Startup song played when Controller starts
void playStartSong() {
  delay(300);  // Short pause after power-on

  // Original upbeat cartoon/game-style startup tune
  playNote(659, 150);  // E5
  playNote(784, 150);  // G5
  playNote(659, 150);  // E5
  playNote(523, 200);  // C5

  playNote(587, 150);  // D5
  playNote(659, 150);  // E5
  playNote(784, 250);  // G5

  playNote(659, 150);  // E5
  playNote(587, 150);  // D5
  playNote(523, 200);  // C5

  playNote(659, 150);  // E5
  playNote(784, 150);  // G5
  playNote(880, 150);  // A5
  playNote(784, 300);  // G5

  // Final note
  playNote(523, 150);  // C5
  playNote(659, 150);  // E5
  playNote(784, 400);  // G5

  // Make absolutely sure the buzzer is silent afterward
  ledcWriteTone(BUZZER_PIN, 0);
}

/**
 * @brief Normalizes Joystick Input to be between -100 and +100
 * @param valve The Joystick Value
 * @param center The Center value for that Joystick
 * @param minimum The Minimum Joystick Value
 * @param maximum The Maximum Joystick Value
 * @return Returns normalized Joystick Value 
 */
int normalizeJoystick(int value, int center, int minimum, int maximum) {
    float output;

    if (value >= center) {
        // Higher than Center 
        output = (float) (value - center) / (maximum - center);
    }
    else {
        // Lower than Center
        output = (float) (value - center) / (center - minimum);
    }
    output *= 100; // Turns a (-1.0 to 1.0) value into a (-100.0 to 100.0) value
    return constrain(output, -100, 100);

}

/**
 * @brief Applys a Deadzone to the normalized Joystick values, making it so slight interference won't cause movement
 * @param value The value the deadzone is being applyed to
 * @param deadzone The Deadzone amount from the center value (0)
 * @return Returns normalized, deadzone added value
 */
int applyDeadzone(int value, int deadzone) {
    if (abs(value) < deadzone) {
        // Value not high enough to be considered a input
        return 0;
    }
    return value;
}

/**
 * @brief Inverses the given value (->+ or +>-)
 * @param value Value to be Inverted
 * @return Inverted Value
 */
int inverseValve(float value) {
    // Inverses float value (-1 > 1) (1 > -1)
    if (value < 0) {
        return abs(value);
    }
    else {
        return -(abs(value));
    }
}

  // Runs on startup
void setup() {
  Serial.begin(115200);

  // Set Device as a WiFi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }
  
  esp_now_peer_info_t peerInfo;

  // Register peer
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add receiver");
    return;
  }

  playStartSong();

  delay(500);
}

// Runs Continously
void loop() {

  /* The Unchanged Joystick Data*/
  //data.LeftJoystickX = analogRead(LEFT_X_PIN);
  //data.LeftJoystickY = analogRead(LEFT_Y_PIN);
  //data.LeftButton = digitalRead(LEFT_BUTTON_PIN);

  //data.RightJoystickX = analogRead(RIGHT_X_PIN);
  //data.RightJoystickY = analogRead(RIGHT_Y_PIN);
  //data.RightButton = digitalRead(RIGHT_BUTTON_PIN);


  int LX = normalizeJoystick(analogRead(LEFT_X_PIN), LeftXCenter, Min, Max);
  LX = applyDeadzone(LX, Deadzone);
  LX = inverseValve(LX);    // Needs to be Inversed due to being upsidedown

  int LY = normalizeJoystick(analogRead(LEFT_Y_PIN), LeftYCenter, Min, Max);
  LY = applyDeadzone(LY, Deadzone);
  LY = inverseValve(LY);    // Needs to be Inversed due ot being upsidedown

  int RX = normalizeJoystick(analogRead(RIGHT_X_PIN), RightXCenter, Min, Max);
  RX = applyDeadzone(RX, Deadzone);

  int RY = normalizeJoystick(analogRead(RIGHT_Y_PIN), RightYCenter, Min, Max);
  RY = applyDeadzone(RY, Deadzone);

  data.PacketNumber = packetNumber++;


  // Update Controller Data to be sent
  data.LeftJoystickX = LX;
  data.LeftJoystickY = LY;
  data.RightJoystickX = RX;
  data.RightJoystickY = RY;

  /* Used for Debugging */
  Serial.print("Left X: ");
  Serial.print(data.LeftJoystickX);
  Serial.print(" Left Y: ");
  Serial.print(data.LeftJoystickY);
  Serial.print(" Right X: ");
  Serial.print(data.RightJoystickX);
  Serial.print(" Right Y: ");
  Serial.print(data.RightJoystickY);
  Serial.print(" Packet # ");
  Serial.println(data.PacketNumber);


esp_now_send(
    receiverAddress,
    (uint8_t *) &data,
    sizeof(data)
  );

  delay(50);  // Send data every 50ms
}