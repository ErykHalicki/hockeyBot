#include <PRIZM.h>    // Include PRIZM Library
#include <HCSR04.h>

PRIZM prizm;//main controller object
EXPANSION exc;//expansion controller object

//faceing the right side at the begining fwd->right
int leftThrottle=50, rightThrottle=50, direction=1,
 connsecutiveDestinationVisits=0, maxVisits=3, currDestination=0,
moveTime=3000;//how long to move for 

void startRoll(int percent1,int percent2) {
	prizm.setMotorPowers(-percent1*direction, percent2*direction);
}

void stopRoll() {
	prizm.setMotorPowers(125, 125);
}

void openGates(){
	exc.setMotorPower(1, 1, 30);
	delay(1500);//not sure how long to open for
}

void closeGates(){//used for what? maybe run the loop for the exact amount of time a match takes then close?
	exc.setMotorPower(1, 1, -30);
	delay(1500);//not sure how long to open for
}

void wait(){
	stopRoll();
	delay(5000);//wait for 5 seconds for testing, 20 seconds for production	
}

void setLocation(){
	//pick new location,light up LED,set correct direction, and start Roll
	bool selected=false;
	int temp;
	while(!selected){
		temp=random(3)-1;
		if(temp!=currDestination)selected=true;	
		else{
			connsecutiveDestinationVisits++;
		}
		if(connsecutiveDestinationVisits<=maxVisits)selected=true;
	}
	direction=max(-1,min(1,temp-currDestination));//setting direction to be either -1, 0 or 1
	startRoll(leftThrottle,rightThrottle);//start the roll in the required direction
	delay(moveTime*abs(temp-currDestination));//wait the amount of time it takes to cross half the court times the amount to move (0, 1 or 2 after abs)
	currDestination=temp;//once moved, set current location as temp
}

void setup(){
	prizm.PrizmBegin();
  	Serial.begin(115200);
	openGates();
}
int loops=0;
void loop(){
	if(loops<10){
		setLocation();
		wait();
	}
	else if(loops==10){
		closeGates();
	}
	loops++;
}

