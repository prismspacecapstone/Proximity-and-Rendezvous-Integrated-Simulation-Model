#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <LiquidCrystal_I2C.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

// BNO055 sensor setup
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Calibration offsets
float w = 0;
float x = 0;
float y = 0;
float z = 0;

/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
void displayCalStatus(void)
{
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }

  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
  Serial.begin(115200);
  
  // Initialize LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  while (!Serial) delay(10);

  Serial.println("Orientation Sensor Test"); 
  Serial.println("");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BNO055 Error!");
    lcd.setCursor(0, 1);
    lcd.print("Check Wiring");
    while(1);
  }

  delay(1000);

  bno.setExtCrystalUse(true);

  Calibrate();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready!");
  delay(1000);
}

/**************************************************************************/
/*
    Main loop
*/
/**************************************************************************/
void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);

  // Calculate relative orientations
  float xr, yr, zr;
  
  if(event.orientation.x - x >= 360){
    xr = event.orientation.x - x - 360;
  }else{
    xr = event.orientation.x - x;
  }

  if(event.orientation.y - y >= 90){
    yr = 90 - (event.orientation.y - y - 90);
  }else if(event.orientation.y - y <= -90){
    yr = -90 - (event.orientation.y - y + 90);
  }else{
    yr = event.orientation.y - y;
  }

  if(event.orientation.z - z <= -180){
    zr = event.orientation.z - z + 360;
  }else if(event.orientation.z - z >= 180){
    zr = event.orientation.z - z - 360;
  }else{
    zr = event.orientation.z - z;
  }

  // Display on Serial Monitor
  Serial.print("Xr: ");
  Serial.print(xr, 4);
  Serial.print("\tYr: ");
  Serial.print(yr, 4);
  Serial.print("\tZr: ");
  Serial.print(zr, 4);
  Serial.print("\tX: ");
  Serial.print(event.orientation.x, 4);
  Serial.print("\tY: ");
  Serial.print(event.orientation.y, 4);
  Serial.print("\tZ: ");
  Serial.print(event.orientation.z, 4);
  
  displayCalStatus();
  Serial.println("");

  // Display on LCD (showing relative orientation)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("X:");
  lcd.print(xr, 1);  // 1 decimal place to fit more data
  lcd.print(" Y:");
  lcd.print(yr, 1);
  
  lcd.setCursor(0, 1);
  lcd.print("Z:");
  lcd.print(zr, 1);
  
  // Optional: Display calibration status on LCD
  uint8_t system, gyro, accel, mag;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  lcd.print(" S:");
  lcd.print(system);

  /* Wait the specified delay before requesting next data */
  delay(BNO055_SAMPLERATE_DELAY_MS);

  // Check for calibration reset button
  if(digitalRead(31)){
    CalibrateEuler(event);
  }
}

/**************************************************************************/
/*
    Reset orientation to current position
*/
/**************************************************************************/
void CalibrateEuler(sensors_event_t e){
  Serial.println("reset");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resetting...");
  
  x = e.orientation.x;
  y = e.orientation.y;
  z = e.orientation.z;
  
  delay(500);
}

/**************************************************************************/
/*
    Calibration routine
*/
/**************************************************************************/
void Calibrate(void){
  bool isCalibrated = false;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");
  
  while(!isCalibrated){
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);

    Serial.print("\t");
    if (!system)
    {
      Serial.print("! ");
    }

    sensors_event_t event;
    bno.getEvent(&event);

    // Display calibration status on Serial
    Serial.print("Sys:");
    Serial.print(system, DEC);
    Serial.print(" G:");
    Serial.print(gyro, DEC);
    Serial.print(" A:");
    Serial.print(accel, DEC);
    Serial.print(" M:");
    Serial.print(mag, DEC);
    Serial.print("\tX: ");
    Serial.print(event.orientation.x, 4);
    Serial.print("\tY: ");
    Serial.print(event.orientation.y, 4);
    Serial.print("\tZ: ");
    Serial.print(event.orientation.z, 4);
    Serial.println("");

    // Display calibration status on LCD
    lcd.setCursor(0, 1);
    lcd.print("S:");
    lcd.print(system);
    lcd.print(" G:");
    lcd.print(gyro);
    lcd.print(" M:");
    lcd.print(mag);

    delay(BNO055_SAMPLERATE_DELAY_MS);

    if((system == 3) && (gyro == 3) && (mag == 3)){
      Serial.println("\nCalibration Complete\n");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Calibration");
      lcd.setCursor(0, 1);
      lcd.print("Complete!");
      delay(2000);
      isCalibrated = true;
    }
  }
}
