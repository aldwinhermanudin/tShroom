#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>

#define DHTPIN 22
#define PUMPPIN 23
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

void setup() {
  delay(100);
  Serial.begin(9600);
  Serial.println("--------tShroom--------\n");

  pinMode(PUMPPIN, OUTPUT);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
  }
  dht.begin();
}

float humid, temp, pressure;

void loop() {
  humid = dht.readHumidity();
  temp = (dht.readTemperature() + bmp.readTemperature()) / 2;
  pressure = bmp.readPressure();

  // Check if any reads failed.
  if (isnan(humid) || isnan(temp) || isnan(pressure)) {
    Serial.println("Failed to read from sensor!");
    digitalWrite(PUMPPIN, LOW);
  }
  else {
    if (humid < 75) {
      digitalWrite(PUMPPIN, HIGH);
    }
    else {
      digitalWrite(PUMPPIN, LOW);
    }
    Serial.print("Humidity: ");
    Serial.print(humid);
    Serial.println(" %");
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" *C");
    Serial.print("Pressure = ");
    Serial.print(pressure);
    Serial.println(" Pa\n");
  }
  delay(2000);

}
