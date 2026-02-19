/*
 * MockSat v3.1 - Teensy 4.1 with HM-10 Bluetooth Module
 * 
 * Message Protocol:
 *   Telemetry (current position):  "TEL:x,y,theta"   e.g. "TEL:1.5,3.2,90.0"
 *   Command   (target position):   "CMD:x,y,theta"   e.g. "CMD:5.0,2.1,45.0"
 * 
 * Hardware Connections:
 * HM-10 VCC -> Teensy 3.3V
 * HM-10 GND -> Teensy GND
 * HM-10 TXD -> Teensy Pin 0 (RX1)
 * HM-10 RXD -> Teensy Pin 1 (TX1)
 */

HardwareSerial &BLE = Serial1;

// --- Positional State ---
struct Position {
  float x;
  float y;
  float theta;
};

Position currentPos = {0.0, 0.0, 0.0};   // Updated by telemetry
Position targetPos  = {0.0, 0.0, 0.0};   // Updated by command

bool newTelemetry = false;
bool newCommand   = false;

// --- Message buffer ---
#define MSG_BUFFER_SIZE 128
char msgBuffer[MSG_BUFFER_SIZE];
int  msgIndex = 0;

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("\n=== MockSat v3.1 ===");

  BLE.begin(9600);
  delay(1000);

  Serial.println("Protocol:");
  Serial.println("  Telemetry -> TEL:x,y,theta");
  Serial.println("  Command   -> CMD:x,y,theta");
  Serial.println("Ready. Waiting for data...\n");
}

void loop() {
  // --- Pass Serial Monitor input to BLE (for manual testing) ---
  while (Serial.available()) {
    char c = Serial.read();
    BLE.write(c);
    Serial.write(c);
  }

  // --- Read BLE input one character at a time, buffer until newline ---
  while (BLE.available() > 0) {
    char c = BLE.read();

    // Ignore carriage returns, treat newline as message terminator
    if (c == '\r') continue;

    if (c == '\n') {
      msgBuffer[msgIndex] = '\0';   // null-terminate
      if (msgIndex > 0) {
        parseMessage(msgBuffer);
      }
      msgIndex = 0;                 // reset for next message
    } else {
      if (msgIndex < MSG_BUFFER_SIZE - 1) {
        msgBuffer[msgIndex++] = c;
      } else {
        // Buffer overflow — discard and reset
        Serial.println("[WARN] Message too long, discarding.");
        msgIndex = 0;
      }
    }
  }

  // --- Act on new data (placeholder for motor/sensor logic) ---
  if (newTelemetry) {
    newTelemetry = false;
    onTelemetryReceived();
  }

  if (newCommand) {
    newCommand = false;
    onCommandReceived();
  }

  delay(5);
}

// ---------------------------------------------------------------
// parseMessage: determines type and extracts x, y, theta
// ---------------------------------------------------------------
void parseMessage(const char* msg) {
  Serial.print("<<< Raw: ");
  Serial.println(msg);

  float x, y, theta;

  if (strncmp(msg, "TEL:", 4) == 0) {
    if (sscanf(msg + 4, "%f,%f,%f", &x, &y, &theta) == 3) {
      currentPos = {x, y, theta};
      newTelemetry = true;
    } else {
      Serial.println("[WARN] Malformed TEL message.");
    }

  } else if (strncmp(msg, "CMD:", 4) == 0) {
    if (sscanf(msg + 4, "%f,%f,%f", &x, &y, &theta) == 3) {
      targetPos = {x, y, theta};
      newCommand = true;
    } else {
      Serial.println("[WARN] Malformed CMD message.");
    }

  } else {
    Serial.print("[WARN] Unknown message type: ");
    Serial.println(msg);
  }
}

// ---------------------------------------------------------------
// Called when a valid telemetry update is received
// ---------------------------------------------------------------
void onTelemetryReceived() {
  Serial.print("[TEL] Current position -> x: ");
  Serial.print(currentPos.x);
  Serial.print("  y: ");
  Serial.print(currentPos.y);
  Serial.print("  theta: ");
  Serial.println(currentPos.theta);
}

// ---------------------------------------------------------------
// Called when a valid movement command is received
// ---------------------------------------------------------------
void onCommandReceived() {
  Serial.print("[CMD] Target position  -> x: ");
  Serial.print(targetPos.x);
  Serial.print("  y: ");
  Serial.print(targetPos.y);
  Serial.print("  theta: ");
  Serial.println(targetPos.theta);

  // TODO: Pass currentPos + targetPos to motion controller
}
