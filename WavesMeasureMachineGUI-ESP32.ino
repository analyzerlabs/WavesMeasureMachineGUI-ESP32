////////////// FOR AUTOUPDATE
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
///////////////// FOR THE MACHINE
#include <ESP32Encoder.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "VGA_GUI.h"
/////////////////////
const String FirmwareVer={"1.0.2"}; 
#define URL_fw_Version "https://raw.githubusercontent.com/analyzerlabs/WavesMeasureMachineGUI-ESP32/master/version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/analyzerlabs/WavesMeasureMachineGUI-ESP32/master/firmware.bin"
HTTPClient http;
const char* ssid = "MIGUEL";
const char* password = "20120415H";
/////////////////////
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
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Waiting for wifi conecction");
  }                                   
  Serial.println("Connected to WiFi");
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



unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 6000;

 
void FirmwareUpdate()
{
  http.begin(URL_fw_Version,"3C 21 AF 58 7F 42 FE C1 37 7B 3E 41 3D C0 F4 7F F1 E7 91 A6");     // PGP KEY TO ACCES TO GITHUB
  delay(200);
  int httpCode = http.GET();            // get data from version file
  delay(200);
  String payload;
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    payload = http.getString();  // save received version
    Serial.println(payload );
  }
  else
  {
    Serial.print("error in downloading version file:");
    Serial.println(httpCode);

  }
  
  http.end();
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
  if(payload.equals(FirmwareVer) )
  {   
     Serial.println("Device already on latest firmware version"); 
  }
  else
  {
     Serial.println("New firmware detected");
     WiFiClient client;

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    //ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);


    t_httpUpdate_return ret = ESPhttpUpdate.update(URL_fw_Bin,"","CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33");
    
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    } 
  }
 }  
}
void repeatedCall(){
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillis) >= interval) 
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      FirmwareUpdate();
    }
 }

 int j=0;
int k=0;
float multiplier = 2*PI/210;
void loop(){
  repeatedCall();
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
