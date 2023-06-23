#include <PRIZM.h>

PRIZM prizm;
EXPANSION exc;

void moveL(int t) {//robot moves to the right for time
  prizm.setMotorPowers(-40, 40);
  delay(t);
  prizm.setMotorPowers(125,125);
}
void moveR(int t) {//robot moves to the left for time
  prizm.setMotorPowers(40, -40);
  delay(t);
  prizm.setMotorPowers(125,125);
}
void openGates(){//gates open
  //exc.setMotorPower(1, 1, 30);
  exc.setMotorDegree(1, 1, 120, 650);
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
  char pos = 'c';//sets robots inital position to centre
  while(true) {
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
}

void jiggle(){
	long start=millis();
	int waitTime=random(13, 17)*1000;
	while(millis()-start<waitTime){
		moveL(500);
		moveR(500);
	}
}
void setup() {
  prizm.PrizmBegin();
  delay(10000); //Delay for starting the bot pregame
  openGates();
  moveR(2700);
  delay(80000); //Delay after the game starts
  moveL(2700);
  
  //closeGates();
  //delay(1000);
}

void loop() {
  int t = 2200;//universal time, can be changed
  chngPos(t);//calls bots movement method
  jiggle();
  //prizm.setMotorPowers(-100, 100);
}
