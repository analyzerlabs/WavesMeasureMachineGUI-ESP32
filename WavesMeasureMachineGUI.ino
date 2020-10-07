
#include <ESP32Encoder.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "VGA_GUI.h"

ESP32Encoder encoder;
//pin configuration
const int redPin = 12;
const int greenPin = 14;
const int bluePin = 27;
const int hsyncPin = 33;
const int vsyncPin = 32;
int Button = 4;
int Color[3]={0,0,0};
String C[3] ={"Red","Green","Blue"}; 
int C_selector=0;
int refreshTime=0;
int refreshTime2=0;
VGA_GUI Interfaz(1);

void setup(){
  Serial.begin(115200);
	//initializing the graphics mode
	Serial.print("free memory: ");
	Serial.print((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
  pinMode(Button,INPUT_PULLUP);
  
	ESP32Encoder::useInternalWeakPullResistors=UP;
  encoder.attachHalfQuad(15, 2);
  encoder.clearCount();
  //SDsetup();
  //sensorSetup();
  pinMode(25,OUTPUT);
  refreshTime=millis();
  refreshTime2=millis();
  Interfaz.fondo("Medidor de Olas");
}

//mainloop

int j=0;
int k=0;
float multiplier = 2*PI/210;
void loop(){
  //Serial.print("Color "+ C[C_selector]+"\t: ");
  //Serial.print(String((int32_t)encoder.getCount()));
  //Serial.println("\t\t Button: " + String(digitalRead(Button)));
  Color[C_selector] = encoder.getCount()/2;
  if(encoder.getCount() == 512 ){
      encoder.setCount(0);
      Color[C_selector]= 0;      
  }
  
  else if(encoder.getCount() == -2 ){
      encoder.setCount(510);  
  }
  
  if(digitalRead(Button)==0){
      while(digitalRead(Button)==0){
        delayMicroseconds(1);
        Serial.print("EWncodfer "+ String((int32_t)encoder.getCount())+"\t: ");
        
        }
      C_selector ++;
      encoder.setCount(Color[C_selector]);
      if(C_selector ==3)C_selector=0;
  }
  if(millis()-refreshTime >5){
      Interfaz.planoCartesiano();
      k = int(50*sin(multiplier*j));
      j++;
      if(j==210)j=0;
      Interfaz.plotRealTime(k);
      refreshTime=millis();      
  }
  
   if(millis()-refreshTime2 >200){
      Interfaz.calcAmplitude();
      Interfaz.printVariables();
      Interfaz.box();
      refreshTime2=millis();      
  }
  
}
