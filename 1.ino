#include <dummy.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32_MailClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Iqooneo6";
const char* password = "a1b2c3d4";

#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465
#define EMAIL_SENDER "yokkeshkumar14@gmail.com"
#define EMAIL_PASSWORD "manigandan"
#define EMAIL_RECIPIENT "yokkeshsanjai2004@gmail.com"

SMTPSession smtp;
Adafruit_MPU6050 mpu;

const int ledPin = 10;
const int pulsePin = A0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(ledPin, OUTPUT);
  pinMode(pulsePin, INPUT);

  if (!mpu.begin(0x68)) {
    if (!mpu.begin(0x69)) {
      scanI2CDevices();
    }
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  smtp.debug(1);
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Accelerometer X: "); Serial.println(a.acceleration.x);
  Serial.print("Accelerometer Y: "); Serial.println(a.acceleration.y);
  Serial.print("Accelerometer Z: "); Serial.println(a.acceleration.z);
  Serial.print("Temperature: "); Serial.println(temp.temperature);

  int pulseValue = analogRead(pulsePin);
  Serial.print("Pulse Sensor Value: ");
  Serial.println(pulseValue);

  if (abs(a.acceleration.x) > 15 || abs(a.acceleration.y) > 15 || abs(a.acceleration.z) > 15) {
    blinkLED();
    sendEmailAlert("Abnormal acceleration detected");
  }

  if (temp.temperature > 40 || temp.temperature < 10) {
    blinkLED();
    sendEmailAlert("Abnormal temperature detected");
  }

  if (pulseValue > 130 || pulseValue < 100) {
    blinkLED();
    sendEmailAlert("Abnormal pulse detected");
  }

  delay(5000);
}

void blinkLED() {
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
}

void sendEmailAlert(String alertMessage) {
  if (WiFi.status() == WL_CONNECTED) {
    SMTPMessage message;
    message.sender.name = "Yokkesh Kumar";
    message.sender.email = EMAIL_SENDER;
    message.subject = "Emergency Alert - Abnormal Condition Detected";
    message.addRecipient("Sanjai", EMAIL_RECIPIENT);
    message.text.content = "Alert: " + alertMessage + " Immediate attention required!";

    if (!MailClient.sendMail(smtp, message)) {
      Serial.println("Error sending email: " + smtp.errorReason());
    } else {
      Serial.println("Email sent successfully");
    }
  } else {
    Serial.println("Wi-Fi not connected");
  }
}

void scanI2CDevices() {
  byte error, address;
  int nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  if (nDevices == 0) Serial.println("No I2C devices found.");
  else Serial.print("Total I2C devices found: ");
  Serial.println(nDevices);
}
