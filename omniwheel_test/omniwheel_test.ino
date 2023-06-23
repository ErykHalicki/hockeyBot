#include <PRIZM.h>      // Include the PRIZM library in sketch

PRIZM prizm;            // Instantiate an object in the PRIZM class named "prizm".

EXPANSION exc;  
void setup() {
  prizm.PrizmBegin();
}
void powerSet(int powerR, int powerL, int powerS){
  exc.setMotorPowers(1,powerS,0);
  prizm.setMotorPowers(powerL,-powerR); 
}
void loop() {
  //add 5 degrees every half second, maybe itll move in a circle 
  for(int i=0;i<60;i+=1){
    
    if(i<=38){
      powerSet(100,80,0);
    }
    if(i<=60&&i>38){
      powerSet(-22,20,0);
    }
    delay(100);
  }
}
