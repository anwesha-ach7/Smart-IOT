#define BLYNK_TEMPLATE_ID "TMPL69ZvtYmoi"
#define BLYNK_TEMPLATE_NAME "Test"
#define BLYNK_AUTH_TOKEN "D24XHpBZu2LA2nWzVgKShEZPQkN12SCN"

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

BlynkTimer timer;

// Blynk credentials
char ssid[] = "Sydney_2";
char pass[] = "111111000000";

// DHT11 setup
#define DHTTYPE DHT11
DHT dht1(27, DHTTYPE);

// Soil moisture sensor
#define SOIL_DO_PIN 34  // Digital Output pin

// Ultrasonic sensor
#define TRIG_PIN 32
#define ECHO_PIN 35

// Relay pins
#define RELAY1_PIN 2  // Motor 1 (Ultrasonic triggered)
#define RELAY2_PIN 4  // Motor 2 (Soil moisture triggered)

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); //0x27 lcd ko address ho 
float temp1;
float hum1;
float distanceCm;
String moistureStatus;

void setup() {
  Serial.begin(115200);
  delay(1000);  // Give time for serial to start
  Serial.println("Booting...");
  // Initialize sensors
  dht1.begin();
  pinMode(SOIL_DO_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");

  Blynk.config(BLYNK_AUTH_TOKEN);  // non-blocking
}

void loop() {
  Blynk.run();

  temp1 = dht1.readTemperature();
  hum1 = dht1.readHumidity();

  int soilState = digitalRead(SOIL_DO_PIN);
  moistureStatus = (soilState == HIGH) ? "Dry" : "Wet";

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = duration * 0.034 / 2;

  if (soilState == HIGH) {
    digitalWrite(RELAY2_PIN, HIGH);
    Serial.println("Soil Dry → Motor 2 ON");
  } else {
    digitalWrite(RELAY2_PIN, LOW);
    Serial.println("Soil Wet → Motor 2 OFF");
  }

  if (distanceCm < 3.0) {
    digitalWrite(RELAY1_PIN, HIGH);
    Serial.println("Distance < 3 cm → Motor 1 ON");
  } else {
    digitalWrite(RELAY1_PIN, LOW);
    Serial.println("Distance ≥ 3 cm → Motor 1 OFF");
  }

  Serial.print("Greenhouse Temp: ");
  Serial.println(temp1);
  Serial.print("Greenhouse Humidity: ");
  Serial.println(hum1);
  Serial.print("Soil Status: ");
  Serial.println(moistureStatus);
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T1:");
  lcd.print(temp1, 1);
  lcd.print(" H1:");
  lcd.print(hum1, 1);
  lcd.setCursor(0, 1);
  lcd.print("Soil:");
  lcd.print(moistureStatus);
  lcd.print(" D:");
  lcd.print(distanceCm, 1);
  delay(2000);

  Blynk.virtualWrite(V1, temp1);
  Blynk.virtualWrite(V0, distanceCm);
  Blynk.virtualWrite(V7, moistureStatus);
  Blynk.virtualWrite(V8, (digitalRead(RELAY2_PIN) == HIGH) ? "ON" : "OFF");
  Blynk.virtualWrite(V9, (digitalRead(RELAY1_PIN) == HIGH) ? "ON" : "OFF");
  Blynk.virtualWrite(V4, hum1);

  delay(1000);
}
