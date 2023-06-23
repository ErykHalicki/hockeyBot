#include <PRIZM.h>

PRIZM prizm;
EXPANSION exc;

int pwr = 35;
char pos = 'c';

void moveL(int t) {//robot moves to the right for time
  prizm.setMotorPowers(-pwr+5, pwr+35);
  delay(t-120);
  prizm.setMotorPowers(0,0);
  delay(300);
}
void moveR(int t) {//robot moves to the left for time
  prizm.setMotorPowers(pwr-5, -pwr-35);
  delay(t);
  prizm.setMotorPowers(0,0);
  delay(300);
}
void openGates(){//gates open
  //exc.setMotorPower(1, 1, 30);
  exc.setMotorDegree(1, 1, 200, 650);
  delay(1700);//not sure how long to open for
  //exc.setMotorPower(1, 1, 0);
}
void closeGates(){//gates close
  //exc.setMotorPower(1, 1, -30);
  exc.setMotorDegree(1, 1, 90, 0);
  delay(1650);//not sure how long to open for
  //exc.setMotorPower(1, 1, 0);
}
void chngPos(int t) {//this is how robot decides on movements
  //sets robots inital position to centre
    switch (pos) {
      case 'c'://if bot at centre
        if(random(2)==1) {//move to the right and set position to right
          pos = 'r';
          moveR(t);
        }
        else {//move to the left and set position to left
          pos = 'l';
          moveL(t);
        }
        break;
      case 'l'://if bot at left
        if(random(2)==1) {//move to the centre and set position to centre
          pos = 'c';
          moveR(t);
        }
        else {//move to the right and set position to right
          pos = 'r';
          moveR((t*2)-100);
        }
        break;
      case 'r'://if bot at right
        if(random(2)==1) {//move to the centre and set position to centre
          pos = 'c';
          moveL(t);
        }
        else {//move to the left and set position to left
          pos = 'l';
          moveL((t*2)-100);
        }
        break;
    }
}

void jiggle(){
	long start=millis();
	int waitTime=random(13, 17)*1000;
	while(millis()-start<waitTime){
		//moveL(370);
   		delay(200);
		//moveR(300);
	}
}
void setup() {
  Serial.begin(9600);
  prizm.PrizmBegin();
  delay(12000); //Delay for starting the bot pregame
  openGates();
  //closeGates();
	delay(40000);
}

void loop() {
  int t = 1800;//universal time, can be changed
  chngPos(t);//calls bots movement method
}
