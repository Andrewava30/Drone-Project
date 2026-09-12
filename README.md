# Drone-Project
## Drone Made by Andrew, Kevin, Jorge, and Alex
The Purpose of this repository is track changes on the drone code and to use it as an example to learn from


## Controller
### Parts
|  Part  | Amount  |  Use  |
|  :---:  |  :---:  |  :---:  |
| ESP32 C3 |  1  |  Run code and send to Drone  |
| Joystick  |  2  |  Get Movement Input  |
| Battery 3.7V  |   1  |  Power Controller  |
| On/Off Switch  | 1  | Turn on and off Controller  |
### Wiring
Left X Pin = 0

Left Y Pin = 1

Right X Pin = 2

Right X Pin = 3

Connect 3.3V pin to +

Connect GND pin to -

### Code

All Calculations on inputs are to be done on the Controller side

Send all normalized Left and Right X and Y values from Controller

All values are between -100 and 100
Left = -100
Right = 100
Down = -100
Up = 100

Additionally, Deadzone will be added, so if joystick input is too weak, value will default to 0, no movement


## Drone
### Parts
|  Part  | Amount  |  Use  |
|  :---:  |  :---:  |  :---:  |
| MPU6050  |  1  |  Run code and receive inputs from Controller  |
| Gyroscope  | 1 |  Used to Get information about drone and correct movement  |
| Battery 11.1V  | 1  | Power Drone  |
| ESC  |  4  | Control each motor's speed  |
| Brushless Motor  | 4  | Spins Propellers  |
| Propellers |  4  |  Create Lift to make drone Fly  |
### Wiring

### Motor Directions
Bottom Right = ?
Bottom Left = ?
Top Left = ?
Top Right = ?

### Code

Motor Speed = Throttle + PitchCorrection + RollCorrection - yawCorrection + GyroScopeAdjustments
