#include <HCSR04.h>

#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);
HCSR04 usl(28, 29);//ultrasonic left, right, etc.
HCSR04 usr(24, 25);
HCSR04 usb(26,27);
//trig,echo

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop() {//purpose of program will be to take sensor readings(with some preprocessing) and send to arduino for real processing
  //print out sensor values in a specific order for processing by raspi
  mpu6050.update();
  Serial.print(usl.dist());
  Serial.print(' ');
  Serial.print(usr.dist());
  Serial.print(' ');
  Serial.print(usb.dist());
  Serial.print(' ');
  Serial.print(mpu6050.getAngleX());
  Serial.print(' ');
  Serial.print(mpu6050.getAngleY());
  Serial.print(' ');
  Serial.println(mpu6050.getAngleZ());
  delay(500);
}
