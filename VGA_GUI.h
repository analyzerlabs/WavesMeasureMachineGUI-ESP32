#include <Arduino.h>
#include <ESP32Lib.h>
#include <Ressources/Font6x8.h>
#include <Ressources/CodePage437_9x16.h>

class VGA_GUI:public VGA3BitI,public Mode{
    private:
        const int redPin = 12;
        const int greenPin = 14;
        const int bluePin = 27;
        const int hsyncPin = 33;
        const int vsyncPin = 32;
        Mode myMode = MODE640x480.custom(320, 240);
        int w=320;
        int h=240;
        int sep = 10;
        int xCord = h/3+ 2*h/6- sep/2;
        int i[210];
        int Amplitude = 0;
        int Periodo =0;
        int Longitud = 0;
        int frecuencia = 0;
        int dt = 1 ; //tiempo de ectura entre punto y punto en milisengundos 
        
    public:
        VGA_GUI(){}
        void iniciar(int a){
            setFrameBufferCount(2);
            init(getMode(),this->redPin, this->greenPin, this->bluePin, this->hsyncPin, this->vsyncPin);
            setFont(CodePage437_9x16);
            clear(RGB(0xffffff));
            setCursor(10, 10);
            setTextColor(RGB(0));  
            for(int j=0;j<210;j++){
                this->i[j]=0;
            }
        }

        Mode getMode(){
            return (this->MODE320x240);
        }

        void planoCartesiano(){
            fillRect(this->sep , this->h/4+this->sep , this->w*3/4 - 2*this->sep, this->h*3/4 - 2*this->sep, RGB(0, 255, 255));        
        }

        void box(){
            for(int k=0;k<3;k++){
                fillRect(this->sep+ this->w*3/4 - this->sep , this->h/4+ this->sep + k*this->h/4 - this->sep/2 , (this->w)/4 + 30,  this->h/4 - this->sep/2, RGB(255, 255, 0));  
            }
        }

        void principal(const char *Titulo,int R,int G,int B){
            fillRect(0 , 0 , w, h, RGB(R, G, B));
            setTextColor(RGB(255,255,255));
            setCursor(this->sep,this->sep);
            println(Titulo);
        }

        void fondo(const char *Titulo){
            fillRect(0 , 0 , this->w, this->h, RGB(0, 255, 0));
            setTextColor(RGB(255,255,255));
            setCursor(this->sep,this->sep);
            println(Titulo);                 
        }

        void plotRealTime(int newData){
            this->i[209]=newData;
            short c=0;
            while(c<209){
                fillEllipse(c+this->sep+2,xCord+this->i[c],1,1,RGB(255,0,255));          
                this->i[c] = this->i[c+1];
                c++;
            }          
        }

        void calcAmplitude(){
            int m = this->i[0];
            int x1=0,x2 = 0;
            for(int p=1 ; p<209;p++){ //lee todo los datos , guarda el valor maximo de la ultima cresta
                if(m<this->i[p]){
                    m=this->i[p];
                    x1=p;
                }
            }
            this->Amplitude = m;
            /*for(int p=1 ; p < x1- 20 ;p++){  //asegura leer la primera cresta, y parte inferior de la segunda
                if( this->i[p] >m && m < this->Amplitude){
                    m=this->i[p];
                    x2=p;
                    delayMicroseconds(1);
                }
            }*/
            this->Longitud = abs(x1-x2);
            this->Periodo = (x1-x2)*this->dt;
        }

        void calcPeriodo(){
            int m = this->i[0];
            for(int p=1 ; p<210;p++){
                if(m<this->i[p])m=this->i[p];
            }
            this->Periodo = m;
        }
        void printVariables(){
            fillRect(this->sep , this->h/4-this->sep/2 , this->w*3/4 - 2*this->sep,this->sep/2+ this->sep, RGB(0, 255, 0));         
            setTextColor(RGB(0,0,0));
            setCursor(this->sep,this->sep+30);
            println("Amplitude");
            setCursor(this->sep+50,this->sep+30);
            println("Periodo");
            setTextColor(RGB(0,0,0));
            setCursor(this->sep+30,this->sep+47);
            println(this->Amplitude);    
            setCursor(this->sep+80,this->sep+47);
            println(this->Longitud);     
        }
};
