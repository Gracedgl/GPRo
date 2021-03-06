#include "Arduino.h"
#include "DCMotor.h"

DCMotor::DCMotor(int in1, int in2, int in3, int in4){
  _in1 = in1;
  _in2 = in2;
  _in3 = in3;
  _in4 = in4; 
  
}

void DCMotor::forward(){
  clockwise(_in1,_in2);
  clockwise(_in3,_in4);
}

void DCMotor::backward(){
  antiClockwise(_in1,_in2);
  antiClockwise(_in3,_in4);  
}

void DCMotor::left(){
  antiClockwise(_in1,_in2);
  clockwise(_in3,_in4);  
}

void DCMotor::right(){
  clockwise(_in1,_in2);
  antiClockwise(_in3,_in4);
}

void DCMotor::stop() {
    digitalWrite(_in1, 0); digitalWrite(_in2, 0);
    digitalWrite(_in3, 0); digitalWrite(_in4, 0);  
}

void DCMotor::clockwise(int inx, int iny){
  digitalWrite(inx, 1); digitalWrite(iny, 0);
}
void DCMotor::antiClockwise(int inx, int iny){
  digitalWrite(inx, 0); digitalWrite(iny, 1);
}




