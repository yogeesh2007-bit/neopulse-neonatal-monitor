#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_RED 9
#define LED_GREEN 10
#define LED_BLUE 11
#define BUZZER 5
#define BT_RX 2
#define BT_TX 3

SoftwareSerial bluetooth(BT_RX, BT_TX);

int hr, o2, motion;
float temp;
int riskScore = 0;

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("NeoPulse Hub");
  display.println("Starting...");
  display.display();
}

void loop() {
  if (bluetooth.available()) {
    String data = bluetooth.readStringUntil('\n');
    parseData(data);
    calculateRisk();
    updateDisplay();
    showAlert();
  }
  delay(500);
}

void parseData(String data) {
  int hrPos = data.indexOf("HR:");
  int o2Pos = data.indexOf("O2:");
  int tPos = data.indexOf("T:");
  int mPos = data.indexOf("M:");
  
  hr = data.substring(hrPos+3, data.indexOf(",", hrPos)).toInt();
  o2 = data.substring(o2Pos+3, data.indexOf(",", o2Pos)).toInt();
  temp = data.substring(tPos+2, data.indexOf(",", tPos)).toFloat();
  motion = data.substring(mPos+2).toInt();
}

void calculateRisk() {
  riskScore = 0;
  
  if (hr < 80 || hr > 180) riskScore += 50;
  else if (hr < 100 || hr > 160) riskScore += 20;
  
  if (o2 < 88) riskScore += 50;
  else if (o2 < 93) riskScore += 20;
  
  if (temp < 35.5) riskScore += 50;
  else if (temp < 36.5) riskScore += 20;
  
  if (motion < 10) riskScore += 30;
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("HR:"); display.print(hr); display.println(" bpm");
  display.print("O2:"); display.print(o2); display.println(" %");
  display.print("T:"); display.print(temp); display.println(" C");
  display.print("Risk:"); display.println(riskScore);
  display.display();
}

void showAlert() {
  if (riskScore >= 100) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    tone(BUZZER, 2000, 500);
  } else if (riskScore >= 40) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    tone(BUZZER, 1000, 200);
  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    noTone(BUZZER);
  }
}
