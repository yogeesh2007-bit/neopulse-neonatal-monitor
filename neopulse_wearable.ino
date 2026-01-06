#include <Wire.h>
#include <MAX30105.h>
#include <heartRate.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <MPU6050.h>

#define DS18B20_PIN 2
#define HM10_TX 5
#define HM10_RX 6

MAX30105 particleSensor;
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
SoftwareSerial bt(HM10_RX, HM10_TX);
MPU6050 mpu;

int heartRate = 0;
int o2Sat = 0;
float temp = 0;
int motion = 0;

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
  Wire.begin();
  
  particleSensor.begin();
  particleSensor.setup();
  
  sensors.begin();
  mpu.initialize();
}

void loop() {
  long irValue = particleSensor.getIR();
  
  if (checkForBeat(irValue) == true) {
    heartRate = bpm;
    o2Sat = calculateSpO2();
  }
  
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  motion = sqrt(ax*ax + ay*ay + az*az) / 16384.0 * 100;
  
  String data = "HR:" + String(heartRate) + ",O2:" + String(o2Sat) + ",T:" + String(temp) + ",M:" + String(motion);
  bt.println(data);
  
  delay(2000);
}
