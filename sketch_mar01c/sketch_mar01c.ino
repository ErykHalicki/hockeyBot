#include <PRIZM.h>
PRIZM prizm;
int i=180;
void setup() {
  // put your setup code here, to run once:
  prizm.PrizmBegin();
  Serial.begin(115200);
  prizm.setServoPosition(1,0);
  prizm.setServoPosition(2,i);//claw
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(i);
  prizm.setServoPosition(2,i);//claw
  if(Serial.available()>=4){
    i=Serial.parseInt();
  }
  delay(50);
}
