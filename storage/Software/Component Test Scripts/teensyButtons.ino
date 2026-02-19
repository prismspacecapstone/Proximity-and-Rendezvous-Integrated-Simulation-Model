// Button pins
const int K1 = 14;
const int K2 = 15;
const int K3 = 16;
const int K4 = 17;

// Button state variables
int lastK1State = HIGH;
int lastK2State = HIGH;
int lastK3State = HIGH;
int lastK4State = HIGH;

// Debounce timing
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Configure button pins as inputs with internal pull-up resistors
  pinMode(K1, INPUT_PULLUP);
  pinMode(K2, INPUT_PULLUP);
  pinMode(K3, INPUT_PULLUP);
  pinMode(K4, INPUT_PULLUP);
  
  Serial.println("Button Monitor Ready");
  Serial.println("Waiting for button press...");
}

void loop() {
  // Read current button states
  int k1State = digitalRead(K1);
  int k2State = digitalRead(K2);
  int k3State = digitalRead(K3);
  int k4State = digitalRead(K4);
  
  // Check K1 - only trigger on press (HIGH to LOW transition)
  if (k1State != lastK1State) {
    lastDebounceTime = millis();
    if (k1State == LOW) {
      Serial.println("K1 pressed");
    }
  }
  lastK1State = k1State;
  
  // Check K2
  if (k2State != lastK2State) {
    lastDebounceTime = millis();
    if (k2State == LOW) {
      Serial.println("K2 pressed");
    }
  }
  lastK2State = k2State;
  
  // Check K3
  if (k3State != lastK3State) {
    lastDebounceTime = millis();
    if (k3State == LOW) {
      Serial.println("K3 pressed");
    }
  }
  lastK3State = k3State;
  
  // Check K4
  if (k4State != lastK4State) {
    lastDebounceTime = millis();
    if (k4State == LOW) {
      Serial.println("K4 pressed");
    }
  }
  lastK4State = k4State;
  
  delay(10); // Small delay to prevent excessive polling
}
