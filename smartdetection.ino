
// Smart Helmet — crash detection (minimal, no debug prints)
#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11); // HC-05: TX->D10, RX<-D11
const uint8_t MPU_ADDR = 0x68;

const int WEAR_PIN = 2;
const int BUZZER = 8;

bool rideMode = false;
bool alarmOn = false;
unsigned long lastAlarm = 0;
const unsigned long LOCK_MS = 10000UL; // 10s

// detection tuning (adjust after testing)
const float ACCEL_SCALE = 16384.0f; // ±2g scale LSB/g
const float IMPACT_THRESHOLD_G = 2.5f; // set between ~2.0 - 3.5
const unsigned long CONFIRM_MS = 2000UL; // confirm window
const float STILL_TOL = 0.30f; // ±0.3g around 1.0
const float TILT_ANGLE = 45.0f; // degrees

// I2C helpers
void mpuWrite(uint8_t r, uint8_t v){ Wire.beginTransmission(MPU_ADDR); Wire.write(r); Wire.write(v); Wire.endTransmission(); }
void mpuRead(uint8_t r, uint8_t n, uint8_t *b){ Wire.beginTransmission(MPU_ADDR); Wire.write(r); Wire.endTransmission(false); Wire.requestFrom(MPU_ADDR,n); uint8_t i=0; while(Wire.available() && i<n) b[i++]=Wire.read(); }

void mpuInit(){
  mpuWrite(0x6B, 0x00); // wake
  delay(10);
  mpuWrite(0x1C, 0x00); // accel ±2g
  delay(10);
}

void readAccelRaw(int16_t &ax,int16_t &ay,int16_t &az){
  uint8_t b[6]; mpuRead(0x3B,6,b);
  ax = ((int16_t)b[0]<<8)|b[1];
  ay = ((int16_t)b[2]<<8)|b[3];
  az = ((int16_t)b[4]<<8)|b[5];
}

float accelMagG(int16_t axr,int16_t ayr,int16_t azr){
  float ax = axr/ACCEL_SCALE, ay = ayr/ACCEL_SCALE, az = azr/ACCEL_SCALE;
  return sqrt(ax*ax + ay*ay + az*az);
}

void accelToPitchRoll(int16_t axr,int16_t ayr,int16_t azr, float &pitch, float &roll){
  float ax=axr/ACCEL_SCALE, ay=ayr/ACCEL_SCALE, az=azr/ACCEL_SCALE;
  pitch = atan2(ax, sqrt(ay*ay + az*az)) * 180.0 / PI;
  roll  = atan2(ay, sqrt(ax*ax + az*az)) * 180.0 / PI;
}

bool detectImpact(){
  int16_t ax,ay,az; readAccelRaw(ax,ay,az);
  float mag = accelMagG(ax,ay,az);
  return (mag >= IMPACT_THRESHOLD_G);
}

bool confirmCrash(){
  unsigned long start = millis();
  const unsigned long sampleDelay = 100UL;
  int stillCount = 0, samples = 0;
  int samplesNeeded = CONFIRM_MS / sampleDelay;

  while (millis() - start < CONFIRM_MS){
    int16_t ax,ay,az; readAccelRaw(ax,ay,az);
    float mag = accelMagG(ax,ay,az);
    if (fabs(mag - 1.0f) < STILL_TOL) stillCount++;
    float pitch, roll; accelToPitchRoll(ax,ay,az,pitch,roll);
    if (fabs(pitch) > TILT_ANGLE || fabs(roll) > TILT_ANGLE) return true;
    samples++; delay(sampleDelay);
  }
  return (stillCount >= (int)(samplesNeeded * 0.6f));
}

void triggerAlarm(){
  alarmOn = true;
  BT.println("🚨 SOS! Possible crash detected!");
  for (int i=0;i<6;i++){
    digitalWrite(BUZZER, HIGH);
    delay(400);
    digitalWrite(BUZZER, LOW);
    delay(300);
  }
  alarmOn = false;
}

void setup(){
  pinMode(WEAR_PIN, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  Wire.begin();
  BT.begin(9600);
  mpuInit();
  // optional: check WHO_AM_I separately before demo
}

void loop(){
  bool worn = !digitalRead(WEAR_PIN);

  if (worn && !rideMode){ rideMode = true; BT.println("INFO: Helmet worn"); }
  else if (!worn && rideMode){ rideMode = false; BT.println("INFO: Helmet removed"); }

  if (rideMode && !alarmOn && (millis() - lastAlarm > LOCK_MS)){
    if (detectImpact()){
      if (confirmCrash()){
        triggerAlarm();
        lastAlarm = millis();
      }
    }
  }
  delay(40);
}