#include <PRIZM.h>    // Include PRIZM Library
#include <HCSR04.h>
#include <Pixy2I2C.h>

HCSR04 usf(2, 9);//ultrasonic left, right, etc.
HCSR04 usl(17, 3);
HCSR04 usr(4, 5);
PRIZM prizm;//main controller object
EXPANSION exc;//expansion controller object
Pixy2I2C pixy;

const int FULL_TURN_DEGREES = 1180; //degrees it takes to do a 180, or a 360 if both wheels turn
const int TURN_SPEED = 200; //degrees/second
const int HIT_POWER = 100; //degrees to move paddle and retract paddle when hitting
const int INVERTED = -1; //-1 to reverse the turning direction
const int XSIZE=316;
const int YSIZE=208;
const int XDEG=60;
const int YDEG=40;
int orientation=90;
int servoPos=50, servoLower=15,servoUpper=110;
bool rolling=false;
int target=-1;
int dire=1;
int state=0,prevState=state;//0-patrolling, 1- alligning, 2-approaching, 3-collecting(tbd), 4-shooting prep and shot
int avgs[3][5];//0Front,1Left, 2Right
const int AVG_SUM_LEN=5;
const int servoDir=-1;
//0->1->2->3->0 (repeat) ->4->0

void setup() {
  prizm.PrizmBegin();
  Serial.begin(115200);
  Serial.println("Starting...");
  //pixy.init();
  for(int i=0;i<AVG_SUM_LEN;i++){
    updateReadings();
    delay(10);
  }
  //prizm.setServoPosition(2,140);//60 straight, 0 closed, 140 complete 180 open
  //prizm.setServoPosition(3,170);//180 down, 0 up 
}
double getXOffset(int x){
  return (((x)-(XSIZE/2.0))/(XSIZE/2.0))*(XDEG/2.0);
}
double getYOffset(int y){
  return (((y)-(YSIZE/2.0))/(YSIZE/2.0))*-90;
}
int getBalls(){
  pixy.ccc.getBlocks();
  return pixy.ccc.numBlocks;
}
int getClosestBall(){
  if(getBalls()>0){
  int closest=0;
  for (int i=1; i<pixy.ccc.numBlocks; i++){
      if(pixy.ccc.blocks[i].m_width>pixy.ccc.blocks[closest].m_width){
        closest=i;
      }
    } 
  return closest;
}
return -1;
}
void updateReadings(){
  avgs[0][0]=usf.dist();
  avgs[1][0]=usl.dist();
  avgs[2][0]=usr.dist();
  for(int i=0;i<3;i++){
    if(avgs[i][0]==0){
      avgs[i][0]=100;
    }
    
    for(int j=AVG_SUM_LEN-1;j>0;j--){
      avgs[i][j]=avgs[i][j-1];
      //Serial.print(avgs[i][j]);
    }
    //Serial.println();
  }
}
double getLeft() {
  int sum=0;
  for(int i=0;i<AVG_SUM_LEN;i++){
    sum+=avgs[1][i];
  }
  return double(sum)/AVG_SUM_LEN;
}
double getRight() {
  int sum=0;
  for(int i=0;i<AVG_SUM_LEN;i++){
    sum+=avgs[2][i];
  }
  return double(sum)/AVG_SUM_LEN;
}

double getFront(){
  int sum=0;
  for(int i=0;i<AVG_SUM_LEN;i++){
    sum+=avgs[0][i];
  }
  return double(sum)/AVG_SUM_LEN;
}
void turn(double deg) {
  double amt = deg / 360.0;
  long motor1, motor2;
  motor1 = prizm.readEncoderDegrees(1);
  motor2 = prizm.readEncoderDegrees(2);
  prizm.setMotorDegree(1, TURN_SPEED, motor1 + INVERTED * amt * FULL_TURN_DEGREES);
  prizm.setMotorDegree(2, TURN_SPEED, motor2 + INVERTED * amt * FULL_TURN_DEGREES);
  orientation+=deg;
  delay((abs(amt) * FULL_TURN_DEGREES/TURN_SPEED) *1000);
  rolling=false;
}

void startRoll(int percent) {
  if(!rolling){
    prizm.setMotorPowers(-percent, percent);
  }
  rolling=true;
}
void stopRoll() {
  prizm.setMotorPowers(0, 0);
  rolling=false;
}

void hit() {
  exc.setMotorPower(1, 1, -HIT_POWER); //hit the ball
  delay(150);
  exc.setMotorPower(1, 1, 125);
  delay(100);
  exc.setMotorPower(1, 1, HIT_POWER); //reverse
  delay(143);
  exc.setMotorPower(1, 1, 125); //remain in position
}
void up(){
  prizm.setServoPosition(3,30);//up
}
void openclose(){
  prizm.setServoPosition(2,45);//close
  prizm.setServoPosition(1,45);//close
  delay(100);
  up();
  delay(1000);
  prizm.setServoPosition(2,70);//open slightly to drop
  prizm.setServoPosition(1,70);//open slightly to drop
  delay(100);
 }
 void lower(){
  prizm.setServoPosition(3,0);//down
  delay(500);
 }
int behaviour(){
    if(state==0){
      if(getBalls()>0){
        return 1;
      }
      startRoll(50*dire);
      if(getFront()<5){//add getBack function
        dire*=-1;
      }
     return 0;
    }
    if(state==1){
      //pick a distance to move?
      openclose(); 
      int dir=1;
        startRoll(100*dir);
        //blah blah some math based on time moved and distance from wall
      delay(3250);//delay based on how far to move
      stopRoll();
      hit();
      lower();
        //roughly line up with a net?
        return 0;
    }
} 
void safety(){
  if(getFront()<20){
    startRoll(-50);
    delay(400);
    stopRoll();
  }
  if(getRight()<15){
    turn(-40);
    startRoll(-50);
    delay(400);
    stopRoll();
  }
  if(getLeft()<15){
    turn(-0);
    startRoll(-50);
    delay(400);
    stopRoll();
  }
}
void printDist(){
  Serial.print(getFront());
  Serial.print(' ');
  Serial.print(getLeft());
  Serial.print(' ');
  Serial.println(getRight());
}
int i=210;
void loop() {
  state=behaviour();
  if(prevState!=state){
    Serial.println(state);  
    prevState=state;
  }
  updateReadings();
  printDist();
  //safety();//fix safety function
  //correctOrientation();
}
