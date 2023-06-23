#include <HCSR04.h>
#include<Wire.h>
HCSR04 usl(28, 29);//ultrasonic left, right, etc.
HCSR04 usr(24, 25);
HCSR04 usb(26,27);
//trig,echo

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {//purpose of program will be to take sensor readings(with some preprocessing) and send to arduino for real processing
  //print out sensor values in a specific order for processing by raspi

  Serial.print(usl.dist());
  Serial.print(' ');
  Serial.print(usr.dist());
  Serial.print(' ');
  Serial.print(usb.dist());
  
  Wire.beginTransmission(9);//9 is the address of the slave board 
  Wire.write("test");          
  Wire.endTransmission();
  delay(500); 
}
