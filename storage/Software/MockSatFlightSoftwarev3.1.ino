// MockSat v3.1 PRISM Capstone - LCD Menu System with Real IMU + HM-10 Bluetooth
// Teensy 4.1 with LCD1602 I2C, BNO055 IMU, and HM-10 BLE Module
// Hardware Controls: K1=Select, K2=Up, K3=Down, K4=Back/Cancel
// Button pins: K1=14, K2=15, K3=16, K4=17 (shared ground)
//
// HM-10 Bluetooth Connections:
//   HM-10 VCC -> Teensy 3.3V
//   HM-10 GND -> Teensy GND
//   HM-10 TXD -> Teensy Pin 0 (RX1)
//   HM-10 RXD -> Teensy Pin 1 (TX1)
//
// Receive-Only Message Protocol:
//   Telemetry (current position): "TEL:x,y,theta"  e.g. "TEL:1.5,3.2,90.0"
//   Command   (from GTSS):        "CMD:x,y,theta"  e.g. "CMD:5.0,2.1,45.0"
//
// ============================================================
// BLUETOOTH CONNECTION DETECTION NOTE:
//   The MockSat has no hardware pin to detect an HM-10 connection.
//   bluetoothConnected will remain false until the remote station
//   actually SENDS a TEL or CMD message. The command station (GTSS)
//   must transmit data for this unit to know a link is established.
//   Simply pairing or connecting at the BLE layer is NOT sufficient —
//   the remote side must send at least one valid message.
// ============================================================
//
// HM-10 AT Command Reference (sent via BLE.print()):
//   AT             -> test, responds "OK"
//   AT+ROLE0       -> set slave  mode (waits for connection)
//   AT+ROLE1       -> set master mode (can scan/connect)
//   AT+DISC?       -> discover nearby BLE devices (master mode)
//   AT+CON<MAC>    -> connect to device by MAC address
//   AT+RESET       -> reset module (apply role change)
//   AT+ADDR?       -> query own MAC address
//   AT+NAME?       -> query own name
// ============================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

HardwareSerial &BLE = Serial1;

// ====== BUTTON PINS ======
const int K1 = 14;
const int K2 = 15;
const int K3 = 16;
const int K4 = 17;

int lastK1State = HIGH;
int lastK2State = HIGH;
int lastK3State = HIGH;
int lastK4State = HIGH;

// ====== MOTOR CONTROL ======
#define NUM_MOTORS 8
float motorThrottle[NUM_MOTORS] = {0, 0, 0, 0, 0, 0, 0, 0};
bool motorsArmed = false;

// ====== IMU ======
bool imuAvailable = false;
int imuDataScreen = 0;
const int NUM_IMU_SCREENS = 5;

// ====== SETTINGS ======
float defaultThrottle   = 50.0;
int   motorTestDuration = 3;
int   imuUpdateRate     = 100;

// ====== POSITION STATE ======
#define MSG_BUFFER_SIZE 128
char msgBuffer[MSG_BUFFER_SIZE];
int  msgIndex = 0;

// BLE response buffer (for AT command replies)
#define BLE_RESP_SIZE 256
char bleRespBuffer[BLE_RESP_SIZE];
int  bleRespIndex = 0;
String bleLastResponse = "";

struct Position {
  float x;
  float y;
  float theta;
};

// Received over BLE from the remote system
Position telPos  = {0.0, 0.0, 0.0};  // TEL: current position reported by other system
Position cmdPos  = {0.0, 0.0, 0.0};  // CMD: GTSS commanded target

// Local estimated position (set by onboard nav/control logic)
Position localPos = {0.0, 0.0, 0.0}; // Estimated local position (update from nav controller)

bool newTelemetry = false;
bool newCommand   = false;

// ====== BLUETOOTH STATE ======
bool   bluetoothConnected = false; // True only once a valid TEL or CMD message arrives
bool   btIsMaster         = false; // false = slave, true = master
String btMode             = "Slave";

// Device scan results (master mode)
#define MAX_BT_DEVICES 8
String btDevices[MAX_BT_DEVICES];
int    btDeviceCount  = 0;
int    btDeviceCursor = 0;
bool   btScanning     = false;
unsigned long btScanTimer = 0;
#define BT_SCAN_TIMEOUT_MS 6000  // 6 seconds to collect scan results

// ====== MENU STATES ======
enum MenuState {
  BOOT,
  MAIN_MENU,
  PICK_MODE_MENU,
  // Bluetooth
  BLUETOOTH_MENU,
  BLUETOOTH_VIEW,
  BLUETOOTH_MODE,      // Switch slave/master
  BLUETOOTH_SCAN,      // Scanning for devices (master)
  BLUETOOTH_DEVICES,   // Browse scan results and connect
  // Settings
  SETTINGS_MENU,
  SETTINGS_ADJUST_THROTTLE,
  SETTINGS_ADJUST_MOTOR_TEST,
  SETTINGS_ADJUST_IMU_RATE,
  // Diagnostics
  DIAGNOSTICS_MENU,
  DIAGNOSTICS_IMU,
  DIAGNOSTICS_TEMP,
  DIAGNOSTICS_BATTERY,
  DIAGNOSTICS_MOTORS,
  DIAGNOSTICS_POSITION,   // EST local pos + TEL received pos
  DIAGNOSTICS_COMMAND,    // CMD (GTSS target) + TEL received pos
  // System
  CONFIRM_RESET
};

MenuState currentState = BOOT;

int mainMenuCursor      = 0;
int pickModeCursor      = 0;
int btMenuCursor        = 0;
int settingsCursor      = 0;
int diagnosticsCursor   = 0;

float tempThrottle  = 50.0;
int   tempMotorTest = 3;
int   tempIMURate   = 100;

int bootStep = 0;
unsigned long bootTimer = 0;

// ====== IMU DATA ======
float imu_x = 0.0,    imu_y = 0.0,    imu_z = 0.0;
float gyro_x = 0.0,   gyro_y = 0.0,   gyro_z = 0.0;
float accel_x = 0.0,  accel_y = 0.0,  accel_z = 0.0;
float mag_x = 0.0,    mag_y = 0.0,    mag_z = 0.0;
float linear_x = 0.0, linear_y = 0.0, linear_z = 0.0;
float grav_x = 0.0,   grav_y = 0.0,   grav_z = 0.0;
float imu_temp = 0.0;
float x_offset = 0,   y_offset = 0,   z_offset = 0;

String selectedMode = "None";
bool needsRedraw = true;

unsigned long lastSensorUpdate = 0;
unsigned long SENSOR_UPDATE_INTERVAL = 100;

// ====================================================
// SETUP
// ====================================================
void setup() {
  Serial.begin(115200);

  pinMode(K1, INPUT_PULLUP);
  pinMode(K2, INPUT_PULLUP);
  pinMode(K3, INPUT_PULLUP);
  pinMode(K4, INPUT_PULLUP);

  Wire.begin();
  lcd.init();
  lcd.backlight();

  BLE.begin(9600);

  currentState = BOOT;
  bootTimer = millis();

  Serial.println("    ╔════════════════════════════════════════════════════════════════╗");
  Serial.println("    ║                                                                ║");
  Serial.println("    ║             ██████╗ ██████╗ ██╗███████╗███╗   ███╗             ║");
  Serial.println("    ║             ██╔══██╗██╔══██╗██║██╔════╝████╗ ████║             ║");
  Serial.println("    ║             ██████╔╝██████╔╝██║███████╗██╔████╔██║             ║");
  Serial.println("    ║             ██╔═══╝ ██╔══██╗██║╚════██║██║╚██╔╝██║             ║");
  Serial.println("    ║             ██║     ██║  ██║██║███████║██║ ╚═╝ ██║             ║");
  Serial.println("    ║             ╚═╝     ╚═╝  ╚═╝╚═╝╚══════╝╚═╝     ╚═╝             ║");
  Serial.println("    ║                                                                ║");
  Serial.println("    ║        Proximity & Rendezvous Integrated Simulation Model      ║");
  Serial.println("    ║                                                                ║");
  Serial.println("    ║                       Author: Ryan Coder                       ║");
  Serial.println("    ║                      Platform: Teensy 4.1                      ║");
  Serial.println("    ║                                                                ║");
  Serial.println("    ╚════════════════════════════════════════════════════════════════╝");
  Serial.println("=================================");
  Serial.println("Hardware Button Controls:");
  Serial.println("  K1 (Pin 14) = Select");
  Serial.println("  K2 (Pin 15) = Up");
  Serial.println("  K3 (Pin 16) = Down");
  Serial.println("  K4 (Pin 17) = Back/Cancel");
  Serial.println("=================================");
  Serial.println("Bluetooth HM-10 on Serial1:");
  Serial.println("  TEL:x,y,theta  ->  current position (receive)");
  Serial.println("  CMD:x,y,theta  ->  GTSS commanded target (receive)");
  Serial.println("  NOTE: bluetoothConnected stays false until first message");
  Serial.println("        received. Remote must SEND data first.");
  Serial.println("=================================");
  Serial.println();

  randomSeed(analogRead(A0));
}

// ====================================================
// MAIN LOOP
// ====================================================
void loop() {
  handleButtons();
  handleBluetooth();

  if (currentState == BOOT) handleBootSequence();

  // Handle active BLE scan timeout
  if (btScanning && millis() - btScanTimer > BT_SCAN_TIMEOUT_MS) {
    btScanning = false;
    Serial.print("[BT] Scan complete. Found ");
    Serial.print(btDeviceCount);
    Serial.println(" device(s).");
    currentState = BLUETOOTH_DEVICES;
    needsRedraw = true;
  }

  if (newTelemetry) { newTelemetry = false; onTelemetryReceived(); }
  if (newCommand)   { newCommand   = false; onCommandReceived();   }

  updateSensorData();

  if (needsRedraw) { updateDisplay(); needsRedraw = false; }

  delay(10);
}

// ====================================================
// BLUETOOTH
// ====================================================
void handleBluetooth() {
  while (BLE.available() > 0) {
    char c = BLE.read();
    if (c == '\r') continue;

    if (c == '\n') {
      msgBuffer[msgIndex] = '\0';
      if (msgIndex > 0) {
        String line = String(msgBuffer);
        Serial.print("[BT] <<< Raw: "); Serial.println(line);

        // Check if this looks like an AT response (during scan or AT interaction)
        if (btScanning || line.startsWith("OK") || line.startsWith("AT") ||
            line.indexOf(":") > 6 || line.length() == 12) {
          // Likely a scan result (HM-10 format: "OK+DISC:MAC:NAME" or just MAC)
          handleATResponse(line);
        } else {
          // Normal data message — check for connection
          if (!bluetoothConnected) {
            bluetoothConnected = true;
            Serial.println("[BT] Connection detected — first data message received.");
            if (currentState == BLUETOOTH_VIEW) needsRedraw = true;
          }
          parseMessage(msgBuffer);
        }
      }
      msgIndex = 0;
    } else {
      if (msgIndex < MSG_BUFFER_SIZE - 1) {
        msgBuffer[msgIndex++] = c;
      } else {
        Serial.println("[WARN] BLE buffer overflow, discarding.");
        msgIndex = 0;
      }
    }
  }
}

void handleATResponse(String line) {
  Serial.print("[BT] AT Response: "); Serial.println(line);
  bleLastResponse = line;

  // Parse HM-10 scan results
  // Format varies by firmware: "OK+DISC:0" (start), "OK+DIS0:MAC" or just MAC address
  if (btScanning) {
    // Accept lines that look like MAC addresses or device names
    if (line.startsWith("OK+DIS") || line.startsWith("OK+DISC:")) {
      // Extract the part after the last colon as device identifier
      int lastColon = line.lastIndexOf(':');
      if (lastColon >= 0 && btDeviceCount < MAX_BT_DEVICES) {
        String device = line.substring(lastColon + 1);
        device.trim();
        if (device.length() > 0 && device != "0" && device != "1") {
          btDevices[btDeviceCount++] = device;
          Serial.print("[BT] Found device: "); Serial.println(device);
          if (currentState == BLUETOOTH_SCAN) needsRedraw = true;
        }
      }
    }
  }
}

void sendATCommand(String cmd) {
  Serial.print("[BT] >>> AT: "); Serial.println(cmd);
  BLE.print(cmd);
  delay(100);  // Short delay after AT commands
}

void btSetSlave() {
  sendATCommand("AT+ROLE0");
  delay(200);
  sendATCommand("AT+RESET");
  btIsMaster = false;
  btMode = "Slave";
  Serial.println("[BT] Mode set to SLAVE. Module resetting...");
}

void btSetMaster() {
  sendATCommand("AT+ROLE1");
  delay(200);
  sendATCommand("AT+RESET");
  btIsMaster = true;
  btMode = "Master";
  Serial.println("[BT] Mode set to MASTER. Module resetting...");
}

void btStartScan() {
  btDeviceCount = 0;
  btDeviceCursor = 0;
  btScanning = true;
  btScanTimer = millis();
  sendATCommand("AT+DISC?");
  Serial.println("[BT] Scanning for devices...");
}

void btConnectToDevice(int index) {
  if (index < 0 || index >= btDeviceCount) return;
  String mac = btDevices[index];
  // Remove colons from MAC if present (HM-10 wants raw hex string)
  mac.replace(":", "");
  String cmd = "AT+CON" + mac;
  sendATCommand(cmd);
  Serial.print("[BT] Connecting to: "); Serial.println(btDevices[index]);
}

void parseMessage(const char* msg) {
  float x, y, theta;

  if (strncmp(msg, "TEL:", 4) == 0) {
    if (sscanf(msg + 4, "%f,%f,%f", &x, &y, &theta) == 3) {
      telPos = {x, y, theta};
      newTelemetry = true;
    } else {
      Serial.println("[WARN] Malformed TEL message.");
    }
  } else if (strncmp(msg, "CMD:", 4) == 0) {
    if (sscanf(msg + 4, "%f,%f,%f", &x, &y, &theta) == 3) {
      cmdPos = {x, y, theta};
      newCommand = true;
    } else {
      Serial.println("[WARN] Malformed CMD message.");
    }
  } else {
    Serial.print("[WARN] Unknown message type: "); Serial.println(msg);
  }
}

void onTelemetryReceived() {
  Serial.print("[TEL] Received position -> x: "); Serial.print(telPos.x);
  Serial.print("  y: "); Serial.print(telPos.y);
  Serial.print("  theta: "); Serial.println(telPos.theta);
  if (currentState == DIAGNOSTICS_POSITION || currentState == DIAGNOSTICS_COMMAND) needsRedraw = true;
}

void onCommandReceived() {
  Serial.print("[CMD] GTSS target -> x: "); Serial.print(cmdPos.x);
  Serial.print("  y: "); Serial.print(cmdPos.y);
  Serial.print("  theta: "); Serial.println(cmdPos.theta);
  // TODO: Pass cmdPos + localPos to motion controller
  if (currentState == DIAGNOSTICS_COMMAND) needsRedraw = true;
}

// ====================================================
// BUTTON HANDLING
// ====================================================
void handleButtons() {
  int k1State = digitalRead(K1);
  int k2State = digitalRead(K2);
  int k3State = digitalRead(K3);
  int k4State = digitalRead(K4);

  if (k1State != lastK1State) { if (k1State == LOW) handleSelect(); }
  lastK1State = k1State;
  if (k2State != lastK2State) { if (k2State == LOW) handleUp(); }
  lastK2State = k2State;
  if (k3State != lastK3State) { if (k3State == LOW) handleDown(); }
  lastK3State = k3State;
  if (k4State != lastK4State) { if (k4State == LOW) handleBack(); }
  lastK4State = k4State;
}

void handleSelect() {
  Serial.println("[INPUT] K1 - Select");

  switch (currentState) {

    case MAIN_MENU:
      switch (mainMenuCursor) {
        case 0: currentState = PICK_MODE_MENU;   pickModeCursor = 0;    break;
        case 1: currentState = BLUETOOTH_MENU;   btMenuCursor = 0;      break;
        case 2: currentState = SETTINGS_MENU;    settingsCursor = 0;    break;
        case 3: currentState = DIAGNOSTICS_MENU; diagnosticsCursor = 0; break;
        case 4: currentState = CONFIRM_RESET;                           break;
      }
      break;

    case PICK_MODE_MENU:
      {
        String modes[] = { "PointNav", "Xbox Control", "Idle", "Full Throttle",
                           "Half Throttle", "Spin +Z", "Spin -Z", "Other" };
        selectedMode = modes[pickModeCursor];
        Serial.print("[MODE] Selected: "); Serial.println(selectedMode);
        applyMode(pickModeCursor);
        currentState = MAIN_MENU;
      }
      break;

    // ---- BLUETOOTH ----
    case BLUETOOTH_MENU:
      switch (btMenuCursor) {
        case 0:  // View Status
          currentState = BLUETOOTH_VIEW;
          break;
        case 1:  // Switch Mode (slave/master)
          currentState = BLUETOOTH_MODE;
          break;
        case 2:  // Scan for Devices (master only)
          if (btIsMaster) {
            btStartScan();
            currentState = BLUETOOTH_SCAN;
          } else {
            lcd.clear(); lcd.print("Master mode");
            lcd.setCursor(0, 1); lcd.print("required first");
            Serial.println("[BT] Must be in master mode to scan.");
            delay(2000);
            currentState = BLUETOOTH_MENU;
          }
          break;
      }
      break;

    case BLUETOOTH_MODE:
      // K1 toggles the mode and sends AT commands
      if (!btIsMaster) {
        lcd.clear(); lcd.print("Setting Master.."); lcd.setCursor(0, 1); lcd.print("Resetting HM-10");
        btSetMaster();
        delay(1500);
      } else {
        lcd.clear(); lcd.print("Setting Slave..."); lcd.setCursor(0, 1); lcd.print("Resetting HM-10");
        btSetSlave();
        delay(1500);
      }
      currentState = BLUETOOTH_MENU;
      break;

    case BLUETOOTH_SCAN:
      // Pressing select during scan cancels it
      btScanning = false;
      currentState = BLUETOOTH_DEVICES;
      break;

    case BLUETOOTH_DEVICES:
      // K1 on a device attempts to connect
      if (btDeviceCount > 0) {
        lcd.clear(); lcd.print("Connecting...");
        lcd.setCursor(0, 1);
        String shortName = btDevices[btDeviceCursor];
        if (shortName.length() > 16) shortName = shortName.substring(0, 16);
        lcd.print(shortName);
        btConnectToDevice(btDeviceCursor);
        delay(2000);
        currentState = BLUETOOTH_VIEW;
      }
      break;

    // ---- SETTINGS ----
    case SETTINGS_MENU:
      switch (settingsCursor) {
        case 0: tempThrottle = defaultThrottle; currentState = SETTINGS_ADJUST_THROTTLE;   break;
        case 1: tempMotorTest = motorTestDuration; currentState = SETTINGS_ADJUST_MOTOR_TEST; break;
        case 2: tempIMURate = imuUpdateRate; currentState = SETTINGS_ADJUST_IMU_RATE;       break;
        case 3:
          if (imuAvailable) {
            lcd.clear(); lcd.print("Calibrating IMU"); lcd.setCursor(0, 1); lcd.print("Please wait...");
            CalibrateIMU();
            lcd.clear(); lcd.print("IMU Calibrated"); lcd.setCursor(0, 1); lcd.print("Complete!");
            delay(2000);
          } else {
            lcd.clear(); lcd.print("IMU Not Found"); lcd.setCursor(0, 1); lcd.print("Cannot Calibrate");
            delay(2000);
          }
          currentState = SETTINGS_MENU;
          break;
      }
      break;

    case SETTINGS_ADJUST_THROTTLE:
      defaultThrottle = tempThrottle;
      Serial.print("[SETTINGS] Throttle: "); Serial.print(defaultThrottle); Serial.println("%");
      currentState = SETTINGS_MENU; break;
    case SETTINGS_ADJUST_MOTOR_TEST:
      motorTestDuration = tempMotorTest;
      Serial.print("[SETTINGS] Motor Test: "); Serial.print(motorTestDuration); Serial.println("s");
      currentState = SETTINGS_MENU; break;
    case SETTINGS_ADJUST_IMU_RATE:
      imuUpdateRate = tempIMURate;
      SENSOR_UPDATE_INTERVAL = imuUpdateRate;
      Serial.print("[SETTINGS] IMU Rate: "); Serial.print(imuUpdateRate); Serial.println("ms");
      currentState = SETTINGS_MENU; break;

    // ---- DIAGNOSTICS ----
    case DIAGNOSTICS_MENU:
      switch (diagnosticsCursor) {
        case 0: currentState = DIAGNOSTICS_IMU;      imuDataScreen = 0; break;
        case 1: currentState = DIAGNOSTICS_TEMP;     break;
        case 2: currentState = DIAGNOSTICS_BATTERY;  break;
        case 3: currentState = DIAGNOSTICS_MOTORS;   break;
        case 4: currentState = DIAGNOSTICS_POSITION; break;
        case 5: currentState = DIAGNOSTICS_COMMAND;  break;
      }
      break;

    case CONFIRM_RESET:
      lcd.clear(); lcd.print("Resetting...");
      stopAllMotors();
      bluetoothConnected = false;
      telPos  = {0.0, 0.0, 0.0};
      cmdPos  = {0.0, 0.0, 0.0};
      localPos = {0.0, 0.0, 0.0};
      delay(1000);
      currentState = BOOT; bootStep = 0; bootTimer = millis();
      mainMenuCursor = 0; selectedMode = "None";
      Serial.println("[SYSTEM] Reset.");
      break;

    default: break;
  }

  needsRedraw = true;
}

void handleUp() {
  Serial.println("[INPUT] K2 - Up");
  switch (currentState) {
    case MAIN_MENU:          mainMenuCursor--;    if (mainMenuCursor < 0)     mainMenuCursor = 4;    break;
    case PICK_MODE_MENU:     pickModeCursor--;    if (pickModeCursor < 0)     pickModeCursor = 7;    break;
    case BLUETOOTH_MENU:     btMenuCursor--;      if (btMenuCursor < 0)       btMenuCursor = 2;      break;
    case BLUETOOTH_DEVICES:  btDeviceCursor--;    if (btDeviceCursor < 0)     btDeviceCursor = max(0, btDeviceCount - 1); break;
    case SETTINGS_MENU:      settingsCursor--;    if (settingsCursor < 0)     settingsCursor = 3;    break;
    case DIAGNOSTICS_MENU:   diagnosticsCursor--; if (diagnosticsCursor < 0)  diagnosticsCursor = 5; break;
    case SETTINGS_ADJUST_THROTTLE:
      tempThrottle += 5.0; if (tempThrottle > 100.0) tempThrottle = 100.0;
      Serial.print("[SETTINGS] Throttle: "); Serial.print(tempThrottle); Serial.println("%"); break;
    case SETTINGS_ADJUST_MOTOR_TEST:
      tempMotorTest++; if (tempMotorTest > 10) tempMotorTest = 10;
      Serial.print("[SETTINGS] Motor Test: "); Serial.print(tempMotorTest); Serial.println("s"); break;
    case SETTINGS_ADJUST_IMU_RATE:
      tempIMURate -= 50; if (tempIMURate < 50) tempIMURate = 50;
      Serial.print("[SETTINGS] IMU Rate: "); Serial.print(tempIMURate); Serial.println("ms"); break;
    case DIAGNOSTICS_IMU:
      imuDataScreen--; if (imuDataScreen < 0) imuDataScreen = NUM_IMU_SCREENS - 1;
      Serial.print("[IMU] Screen: "); Serial.println(imuDataScreen); break;
    default: break;
  }
  needsRedraw = true;
}

void handleDown() {
  Serial.println("[INPUT] K3 - Down");
  switch (currentState) {
    case MAIN_MENU:          mainMenuCursor++;    if (mainMenuCursor > 4)            mainMenuCursor = 0;    break;
    case PICK_MODE_MENU:     pickModeCursor++;    if (pickModeCursor > 7)            pickModeCursor = 0;    break;
    case BLUETOOTH_MENU:     btMenuCursor++;      if (btMenuCursor > 2)              btMenuCursor = 0;      break;
    case BLUETOOTH_DEVICES:  btDeviceCursor++;    if (btDeviceCursor >= btDeviceCount) btDeviceCursor = 0;  break;
    case SETTINGS_MENU:      settingsCursor++;    if (settingsCursor > 3)            settingsCursor = 0;    break;
    case DIAGNOSTICS_MENU:   diagnosticsCursor++; if (diagnosticsCursor > 5)         diagnosticsCursor = 0; break;
    case SETTINGS_ADJUST_THROTTLE:
      tempThrottle -= 5.0; if (tempThrottle < 0.0) tempThrottle = 0.0;
      Serial.print("[SETTINGS] Throttle: "); Serial.print(tempThrottle); Serial.println("%"); break;
    case SETTINGS_ADJUST_MOTOR_TEST:
      tempMotorTest--; if (tempMotorTest < 1) tempMotorTest = 1;
      Serial.print("[SETTINGS] Motor Test: "); Serial.print(tempMotorTest); Serial.println("s"); break;
    case SETTINGS_ADJUST_IMU_RATE:
      tempIMURate += 50; if (tempIMURate > 500) tempIMURate = 500;
      Serial.print("[SETTINGS] IMU Rate: "); Serial.print(tempIMURate); Serial.println("ms"); break;
    case DIAGNOSTICS_IMU:
      imuDataScreen++; if (imuDataScreen >= NUM_IMU_SCREENS) imuDataScreen = 0;
      Serial.print("[IMU] Screen: "); Serial.println(imuDataScreen); break;
    default: break;
  }
  needsRedraw = true;
}

void handleBack() {
  Serial.println("[INPUT] K4 - Back");
  switch (currentState) {
    case MAIN_MENU: break;
    case PICK_MODE_MENU:
    case BLUETOOTH_MENU:
    case SETTINGS_MENU:
    case DIAGNOSTICS_MENU:
    case CONFIRM_RESET:
      currentState = MAIN_MENU; break;
    case BLUETOOTH_VIEW:
    case BLUETOOTH_MODE:
    case BLUETOOTH_SCAN:
    case BLUETOOTH_DEVICES:
      btScanning = false;
      currentState = BLUETOOTH_MENU; break;
    case SETTINGS_ADJUST_THROTTLE:
    case SETTINGS_ADJUST_MOTOR_TEST:
    case SETTINGS_ADJUST_IMU_RATE:
      Serial.println("[SETTINGS] Cancelled");
      currentState = SETTINGS_MENU; break;
    case DIAGNOSTICS_IMU:
    case DIAGNOSTICS_TEMP:
    case DIAGNOSTICS_BATTERY:
    case DIAGNOSTICS_MOTORS:
    case DIAGNOSTICS_POSITION:
    case DIAGNOSTICS_COMMAND:
      currentState = DIAGNOSTICS_MENU; break;
    default: break;
  }
  needsRedraw = true;
}

// ====================================================
// BOOT SEQUENCE
// ====================================================
void handleBootSequence() {
  unsigned long elapsed = millis() - bootTimer;

  if (bootStep == 0 && elapsed > 0) {
    lcd.clear(); lcd.print("MockSat v3.1"); lcd.setCursor(0, 1); lcd.print("Initializing...");
    Serial.println("[BOOT] MockSat v3.1 Initializing...");
    bootStep++; bootTimer = millis();
  } else if (bootStep == 1 && elapsed > 1500) {
    lcd.clear(); lcd.print("Init Bluetooth"); lcd.setCursor(0, 1); lcd.print("HM-10 Serial1");
    Serial.println("[BOOT] Bluetooth HM-10: Serial1 @ 9600 baud");
    bootStep++; bootTimer = millis();
  } else if (bootStep == 2 && elapsed > 1000) {
    lcd.clear(); lcd.print("Init I2C Wire"); lcd.setCursor(0, 1); lcd.print("Status: OK");
    Serial.println("[BOOT] I2C Wire: OK");
    bootStep++; bootTimer = millis();
  } else if (bootStep == 3 && elapsed > 1000) {
    lcd.clear(); lcd.print("Init IMU"); lcd.setCursor(0, 1);
    if (bno.begin()) {
      imuAvailable = true;
      lcd.print("Status: OK");
      Serial.println("[BOOT] IMU BNO055: OK");
      delay(500);
      bno.setExtCrystalUse(true);
      lcd.clear(); lcd.print("Calibrating IMU"); lcd.setCursor(0, 1); lcd.print("Please wait...");
      CalibrateIMU();
      lcd.clear(); lcd.print("IMU Calibrated"); lcd.setCursor(0, 1); lcd.print("Ready!");
    } else {
      imuAvailable = false;
      lcd.print("Status: N/A");
      Serial.println("[BOOT] IMU: Not detected");
    }
    bootStep++; bootTimer = millis();
  } else if (bootStep == 4 && elapsed > 1500) {
    lcd.clear(); lcd.print("Init Motors"); lcd.setCursor(0, 1); lcd.print("Arming ESCs...");
    armMotors();
    lcd.clear(); lcd.print("Init Motors"); lcd.setCursor(0, 1); lcd.print("Status: OK");
    bootStep++; bootTimer = millis();
  } else if (bootStep == 5 && elapsed > 1000) {
    lcd.clear(); lcd.print("System Ready!"); lcd.setCursor(0, 1); lcd.print("Loading menu...");
    Serial.println("[BOOT] System Ready!");
    bootStep++; bootTimer = millis();
  } else if (bootStep == 6 && elapsed > 1000) {
    currentState = MAIN_MENU;
    needsRedraw = true;
    Serial.println("[BOOT] Boot complete.");
  }
}

// ====================================================
// DISPLAY
// ====================================================
void updateDisplay() {
  lcd.clear();

  switch (currentState) {

    case MAIN_MENU:
      {
        String items[] = { "Pick Mode", "Bluetooth", "Settings", "Diagnostics", "Reset" };
        lcd.print("MockSat v3.1");
        lcd.setCursor(0, 1); lcd.print(">"); lcd.print(items[mainMenuCursor]);
      }
      break;

    case PICK_MODE_MENU:
      {
        String modes[] = { "PointNav", "Xbox Control", "Idle", "Full Throttle",
                           "Half Throttle", "Spin +Z", "Spin -Z", "Other" };
        lcd.print("Select Mode:");
        lcd.setCursor(0, 1); lcd.print(">"); lcd.print(modes[pickModeCursor]);
      }
      break;

    // ---- BLUETOOTH ----
    case BLUETOOTH_MENU:
      {
        String opts[] = { "View Status", "Switch Mode", "Scan Devices" };
        lcd.print("Bluetooth:");
        lcd.setCursor(0, 1); lcd.print(">"); lcd.print(opts[btMenuCursor]);
      }
      break;

    case BLUETOOTH_VIEW:
      // Line 1: connection status
      // Line 2: current BT role
      lcd.print(bluetoothConnected ? "BT: Connected" : "BT: No Signal");
      lcd.setCursor(0, 1);
      lcd.print("Mode: "); lcd.print(btMode);
      break;

    case BLUETOOTH_MODE:
      // Shows current mode and what pressing K1 will do
      lcd.print("Current:");
      lcd.setCursor(0, 1);
      if (!btIsMaster) {
        lcd.print("Slave  K1=Master");
      } else {
        lcd.print("Master K1=Slave ");
      }
      break;

    case BLUETOOTH_SCAN:
      lcd.print("Scanning BLE...");
      lcd.setCursor(0, 1);
      lcd.print("Found: "); lcd.print(btDeviceCount);
      lcd.print("  K1=Stop");
      break;

    case BLUETOOTH_DEVICES:
      if (btDeviceCount == 0) {
        lcd.print("No devices found");
        lcd.setCursor(0, 1); lcd.print("K4=Back");
      } else {
        // Line 1: index indicator
        lcd.print("Dev "); lcd.print(btDeviceCursor + 1);
        lcd.print("/"); lcd.print(btDeviceCount);
        lcd.print(" K1=Conn");
        // Line 2: device identifier (truncated to 16 chars)
        lcd.setCursor(0, 1);
        String d = btDevices[btDeviceCursor];
        if (d.length() > 16) d = d.substring(0, 16);
        lcd.print(d);
      }
      break;

    // ---- SETTINGS ----
    case SETTINGS_MENU:
      {
        String s[] = { "Def Throttle", "Motor Test", "IMU Rate", "Calibrate IMU" };
        lcd.print("Settings:");
        lcd.setCursor(0, 1); lcd.print(">"); lcd.print(s[settingsCursor]);
      }
      break;

    case SETTINGS_ADJUST_THROTTLE:
      lcd.print("Def Throttle:");
      lcd.setCursor(0, 1); lcd.print(tempThrottle, 0); lcd.print("% K1=OK K4=X");
      break;
    case SETTINGS_ADJUST_MOTOR_TEST:
      lcd.print("Motor Test:");
      lcd.setCursor(0, 1); lcd.print(tempMotorTest); lcd.print("s K1=OK K4=X");
      break;
    case SETTINGS_ADJUST_IMU_RATE:
      lcd.print("IMU Rate:");
      lcd.setCursor(0, 1); lcd.print(tempIMURate); lcd.print("ms K1=OK K4=X");
      break;

    // ---- DIAGNOSTICS ----
    case DIAGNOSTICS_MENU:
      {
        // 6 options now
        String d[] = { "IMU Data", "Temp & Mag", "Battery", "Motors", "Position", "Command" };
        lcd.print("Diagnostics:");
        lcd.setCursor(0, 1); lcd.print(">"); lcd.print(d[diagnosticsCursor]);
      }
      break;

    case DIAGNOSTICS_IMU:
      if (imuAvailable) {
        switch (imuDataScreen) {
          case 0:
            lcd.print("Euler X:"); lcd.print(imu_x, 2);
            lcd.setCursor(0, 1); lcd.print("Y:"); lcd.print(imu_y, 2); lcd.print(" Z:"); lcd.print(imu_z, 2); break;
          case 1:
            lcd.print("Gyro X:"); lcd.print(gyro_x, 2);
            lcd.setCursor(0, 1); lcd.print("Y:"); lcd.print(gyro_y, 2); lcd.print(" Z:"); lcd.print(gyro_z, 2); break;
          case 2:
            lcd.print("Accel X:"); lcd.print(accel_x, 2);
            lcd.setCursor(0, 1); lcd.print("Y:"); lcd.print(accel_y, 2); lcd.print(" Z:"); lcd.print(accel_z, 2); break;
          case 3:
            lcd.print("Mag X:"); lcd.print(mag_x, 2);
            lcd.setCursor(0, 1); lcd.print("Y:"); lcd.print(mag_y, 2); lcd.print(" Z:"); lcd.print(mag_z, 2); break;
          case 4:
            lcd.print("Lin X:"); lcd.print(linear_x, 2);
            lcd.setCursor(0, 1); lcd.print("Y:"); lcd.print(linear_y, 2); lcd.print(" Z:"); lcd.print(linear_z, 2); break;
        }
      } else {
        lcd.print("IMU Data"); lcd.setCursor(0, 1); lcd.print("Not Available");
      }
      break;

    case DIAGNOSTICS_TEMP:
      if (imuAvailable) {
        lcd.print("IMU Temp:"); lcd.setCursor(0, 1); lcd.print(imu_temp, 1); lcd.print(" C");
      } else {
        lcd.print("Temp & Mag"); lcd.setCursor(0, 1); lcd.print("Not Available");
      }
      break;

    case DIAGNOSTICS_BATTERY:
      lcd.print("Battery Data"); lcd.setCursor(0, 1); lcd.print("Not Available");
      break;

    case DIAGNOSTICS_MOTORS:
      lcd.print("M1-4:");
      lcd.print(motorThrottle[0], 0); lcd.print(" ");
      lcd.print(motorThrottle[1], 0); lcd.print(" ");
      lcd.print(motorThrottle[2], 0); lcd.print(" ");
      lcd.print(motorThrottle[3], 0);
      lcd.setCursor(0, 1);
      lcd.print("M5-8:");
      lcd.print(motorThrottle[4], 0); lcd.print(" ");
      lcd.print(motorThrottle[5], 0); lcd.print(" ");
      lcd.print(motorThrottle[6], 0); lcd.print(" ");
      lcd.print(motorThrottle[7], 0);
      break;

    case DIAGNOSTICS_POSITION:
      // Line 1: EST = local estimated position (from onboard nav)
      // Line 2: TEL = received telemetry position from remote system
      {
        char line1[17], line2[17];
        snprintf(line1, sizeof(line1), "EST:%.1f,%.1f,%.0f",
                 localPos.x, localPos.y, localPos.theta);
        snprintf(line2, sizeof(line2), "TEL:%.1f,%.1f,%.0f",
                 telPos.x, telPos.y, telPos.theta);
        lcd.print(line1);
        lcd.setCursor(0, 1);
        lcd.print(line2);
      }
      break;

    case DIAGNOSTICS_COMMAND:
      // Line 1: CMD = GTSS commanded target position
      // Line 2: TEL = received current position (for comparison)
      {
        char line1[17], line2[17];
        snprintf(line1, sizeof(line1), "CMD:%.1f,%.1f,%.0f",
                 cmdPos.x, cmdPos.y, cmdPos.theta);
        snprintf(line2, sizeof(line2), "TEL:%.1f,%.1f,%.0f",
                 telPos.x, telPos.y, telPos.theta);
        lcd.print(line1);
        lcd.setCursor(0, 1);
        lcd.print(line2);
      }
      break;

    case CONFIRM_RESET:
      lcd.print("Reset System?"); lcd.setCursor(0, 1); lcd.print("K1=Yes K4=No");
      break;

    default: break;
  }
}

// ====================================================
// SENSOR DATA
// ====================================================
void updateSensorData() {
  if (imuAvailable && millis() - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
    sensors_event_t orientationData, angVelocityData, linearAccelData,
                    magnetometerData, accelerometerData, gravityData;

    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    imu_x = orientationData.orientation.x - x_offset;
    imu_y = orientationData.orientation.y - y_offset;
    imu_z = orientationData.orientation.z - z_offset;
    if (imu_x >= 180)       imu_x -= 360;
    else if (imu_x < -180)  imu_x += 360;
    if (imu_z >= 180)       imu_z -= 360;
    else if (imu_z < -180)  imu_z += 360;

    bno.getEvent(&angVelocityData,   Adafruit_BNO055::VECTOR_GYROSCOPE);
    gyro_x = angVelocityData.gyro.x; gyro_y = angVelocityData.gyro.y; gyro_z = angVelocityData.gyro.z;

    bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
    accel_x = accelerometerData.acceleration.x; accel_y = accelerometerData.acceleration.y; accel_z = accelerometerData.acceleration.z;

    bno.getEvent(&magnetometerData,  Adafruit_BNO055::VECTOR_MAGNETOMETER);
    mag_x = magnetometerData.magnetic.x; mag_y = magnetometerData.magnetic.y; mag_z = magnetometerData.magnetic.z;

    bno.getEvent(&linearAccelData,   Adafruit_BNO055::VECTOR_LINEARACCEL);
    linear_x = linearAccelData.acceleration.x; linear_y = linearAccelData.acceleration.y; linear_z = linearAccelData.acceleration.z;

    bno.getEvent(&gravityData,       Adafruit_BNO055::VECTOR_GRAVITY);
    grav_x = gravityData.acceleration.x; grav_y = gravityData.acceleration.y; grav_z = gravityData.acceleration.z;

    imu_temp = bno.getTemp();
    lastSensorUpdate = millis();

    if (currentState == DIAGNOSTICS_IMU   || currentState == DIAGNOSTICS_TEMP ||
        currentState == DIAGNOSTICS_BATTERY || currentState == DIAGNOSTICS_MOTORS) {
      needsRedraw = true;
    }
  }
}

// ====================================================
// IMU CALIBRATION
// ====================================================
void CalibrateIMU() {
  bool isCalibrated = false;
  while (!isCalibrated) {
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);
    lcd.setCursor(0, 1);
    lcd.print("S:"); lcd.print(system); lcd.print(" G:"); lcd.print(gyro); lcd.print(" M:"); lcd.print(mag); lcd.print("   ");
    delay(BNO055_SAMPLERATE_DELAY_MS);
    if ((system == 3) && (gyro == 3) && (mag == 3)) {
      isCalibrated = true;
      sensors_event_t event;
      bno.getEvent(&event);
      x_offset = event.orientation.x;
      y_offset = event.orientation.y;
      z_offset = event.orientation.z;
      Serial.println("\nCalibration Complete");
      Serial.print("Zero ref - X:"); Serial.print(x_offset); Serial.print(" Y:"); Serial.print(y_offset); Serial.print(" Z:"); Serial.println(z_offset);
    }
  }
}

// ====================================================
// MOTOR CONTROL
// ====================================================
void setMotorThrottle(int motorNum, float throttle) {
  if (motorNum < 1 || motorNum > NUM_MOTORS) { Serial.print("[MOTOR] Invalid: "); Serial.println(motorNum); return; }
  if (!motorsArmed) { Serial.println("[MOTOR] Not armed!"); return; }
  throttle = constrain(throttle, 0, 100);
  motorThrottle[motorNum - 1] = throttle;
  Serial.print("[MOTOR] M"); Serial.print(motorNum); Serial.print(" -> "); Serial.print(throttle); Serial.println("%");
  // TODO: PWM output via HiLetgo PWM board
}
void setAllMotors(float t)  { for (int i = 1; i <= NUM_MOTORS; i++) setMotorThrottle(i, t); }
void setEvenMotors(float t) { for (int i = 2; i <= NUM_MOTORS; i += 2) setMotorThrottle(i, t); }
void setOddMotors(float t)  { for (int i = 1; i <= NUM_MOTORS; i += 2) setMotorThrottle(i, t); }
void stopAllMotors()        { Serial.println("[MOTOR] Stop"); for (int i = 0; i < NUM_MOTORS; i++) motorThrottle[i] = 0; }

void armMotors() {
  Serial.println("[MOTOR] Arming ESCs...");
  for (int i = 0; i < NUM_MOTORS; i++) motorThrottle[i] = 0;
  motorsArmed = true;
  Serial.println("[MOTOR] ESCs armed");
}
void disarmMotors() { stopAllMotors(); motorsArmed = false; }

void applyMode(int m) {
  stopAllMotors();
  switch (m) {
    case 0: Serial.println("[MODE] PointNav"); break;
    case 1: Serial.println("[MODE] Xbox Controller"); break;
    case 2: Serial.println("[MODE] Idle"); break;
    case 3: setAllMotors(100); Serial.println("[MODE] Full Throttle"); break;
    case 4: setAllMotors(defaultThrottle); Serial.print("[MODE] Half Throttle "); Serial.print(defaultThrottle); Serial.println("%"); break;
    case 5: setEvenMotors(defaultThrottle); Serial.println("[MODE] Spin +Z"); break;
    case 6: setOddMotors(defaultThrottle);  Serial.println("[MODE] Spin -Z"); break;
    case 7: Serial.println("[MODE] Other"); break;
  }
}
