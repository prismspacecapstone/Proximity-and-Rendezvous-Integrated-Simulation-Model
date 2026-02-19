#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.

   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).

   Connections
   ===========
   Connect SCL to 19
   Connect SDA to 18
   Connect VDD to 3-5V DC (depending on your board's logic level)
   Connect GROUND to common ground

   History
   =======
   2015/MAR/03  - First release (KTOWN)
   2015/AUG/27  - Added calibration and system status helpers
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);
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
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }

  /* Display the individual values */
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

  while (!Serial) delay(10);  // wait for serial port to open!

  Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  bno.setExtCrystalUse(true);

  Calibrate();
}

/**************************************************************************/
/*
    Euler Coord
*/
/**************************************************************************/
void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);

  /* Display the floating point data */

  if(event.orientation.x - x >= 360){
    Serial.print("Xr: ");
    Serial.print(event.orientation.x - x - 360, 4);
  }else{
    Serial.print("Xr: ");
    Serial.print(event.orientation.x - x, 4);
  }


  //fix this bullshit ass code
  if(event.orientation.y - y >= 90){
    Serial.print("\tYr: ");
    Serial.print(90 - (event.orientation.y - y - 90), 4);
  }else if(event.orientation.y - y <=-90){
    Serial.print("\tYr: ");
    Serial.print(-90 - (event.orientation.y - y + 90), 4);
  }else{
    Serial.print("\tYr: ");
    Serial.print(event.orientation.y - y, 4);
  }



  if(event.orientation.z - z <= -180){
    Serial.print("\tZr: ");
    Serial.print(event.orientation.z - z + 360, 4);
  }else if(event.orientation.z - z >= 180){
    Serial.print("\tZr: ");
    Serial.print(event.orientation.z - z - 360, 4);
  }else{
    Serial.print("\tZr: ");
    Serial.print(event.orientation.z - z, 4);
  }



  Serial.print("\tX: ");
  Serial.print(event.orientation.x , 4);
  Serial.print("\tY: ");
  Serial.print(event.orientation.y, 4);
  Serial.print("\tZ: ");
  Serial.print(event.orientation.z, 4);
  /* Optional: Display calibration status */
  displayCalStatus();

  /* New line for the next sample */
  Serial.println("");
  /* Wait the specified delay before requesting nex data */
  delay(BNO055_SAMPLERATE_DELAY_MS);


  // look for pot value to begin a zero out
  if(digitalRead(31)){
    CalibrateEuler(event);
  }
  
}

void CalibrateEuler(sensors_event_t e){
  Serial.println("reset");
  x = e.orientation.x;

  y = e.orientation.y;
  z = e.orientation.z;
}

void Calibrate(void){
  bool isCalibrated = false;
  while(!isCalibrated){
    uint8_t system, gyro, accel, mag;
    system = gyro = accel = mag = 0;
    bno.getCalibration(&system, &gyro, &accel, &mag);

    /* The data should be ignored until the system calibration is > 0 */
    Serial.print("\t");
    if (!system)
    {
      Serial.print("! ");
    }

    sensors_event_t event;
    bno.getEvent(&event);

    /* Display the individual values */
    Serial.print("Sys:");
    Serial.print(system, DEC);
    Serial.print(" G:");
    Serial.print(gyro, DEC);
    Serial.print(" A:");
    Serial.print(accel, DEC);
    Serial.print(" M:");
    Serial.print(mag, DEC);

    Serial.print("\tX: ");
    Serial.print(event.orientation.x , 4);
    Serial.print("\tY: ");
    Serial.print(event.orientation.y, 4);
    Serial.print("\tZ: ");
    Serial.print(event.orientation.z, 4);

    Serial.println("");

    

    delay(BNO055_SAMPLERATE_DELAY_MS);

    if((system == 3) && (gyro == 3) && (mag == 3)){  //accel calibration is ignored because it's buggy asf
      Serial.println("\nCalibration Complete\n");
      delay(5000);
      isCalibrated = true;
    }
  }
}
