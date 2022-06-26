#include <Wire.h>
#include <MicroLCD.h>
#include <Servo.h>


//Controlador
LCD_SSD1306 oled;
Servo servo;


#define sensObs 2
#define sensTemp A0
#define solenoide 4
#define buzzer 7 
#define led 8 
#define pinServo 12

extern const PROGMEM char frame1,
                     frame2,
                     frame3,
                     frame4,
                     frame5,
                     frame6,
                     frame7,
                     frame8; 

                           
void gif(); 
void piscaFrase(int vezes);
void escreveMensagem(char mensagem, int x, int y);  
void cronometro(int nCronometro);
void Fbuzzer(int quantBuzzer, int tempo);
void Fservo();


// calculo da temperatura media
float amostra = 8;                   // numero de amostras
int acumula = 0 ;
int media = 0;

int d =0;
int vezes; 
float temp = 0.0; 
int valorLido = 0; 
int nCronometro;
bool estadoSensObs; 
int controle; 

void setup() {
  oled.begin();
  pinMode(sensObs, INPUT);
  pinMode(sensTemp, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(solenoide, OUTPUT);
  servo.attach(pinServo);
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH); 
}

void loop() {  
   estadoSensObs = digitalRead(sensObs); 
  
  if (!estadoSensObs){ 
    //bloco verdadeiro
    //espirrar alcool 
      digitalWrite(led, HIGH); 
      Fservo();
      d=0;
      delay(300); 
      controle = 0; 
      int tempMin =28;
      int tempMax = 40; 

      cronometro(5); 
      oled.clear();
       
      if (controle == 1){
      
      
      
      acumula = 0;  //zera a variável acumuladora;
      
      for( media = 0; media < amostra; media++){
        acumula +=  analogRead(sensTemp);
        delay(5);  
      }
      temp = ((acumula / amostra) * 0.488) ;      // Calcula a média de 3 leitura ( 0 = 1  do for )
      
        if ((temp > tempMin) && (temp < tempMax)){
            if (temp > 36.8){
               oled.clear(); 
               oled.setFontSize (FONT_SIZE_MEDIUM);
               oled.setCursor(34, 1);
               oled.print(temp, 1);
               oled.print("C");
               escreveMensagem("Acesso Negado",5, 3);
               Fbuzzer(5, 300);
               delay(3000); 
               oled.clear(); 
            }else if(temp <= 36.8){
               oled.setFontSize (FONT_SIZE_MEDIUM);
               oled.setCursor(34, 1);
               oled.print(temp, 1);
               oled.print("C");
               escreveMensagem("Autorizado",20, 6);
               digitalWrite(solenoide, LOW);
               Fbuzzer(1, 500);  
               delay(3500);
               digitalWrite(solenoide, HIGH);   
               Serial.println(temp);
               delay(3000);
               oled.clear(); 
              // digitalWrite(fotoAcoplador, 1); //abrir tranca
            }   
           }else{
            if ((temp > tempMax) || (temp < tempMin)){
             escreveMensagem("Erro", 44, 3);
             Fbuzzer(1, 800);
             oled.clear();  
             delay(800); 
             cronometro(10); 
             oled.clear();
             escreveMensagem("2 Tentativa",16, 3); 
             delay (2000);
             oled.clear(); 
             cronometro(5);
             oled.clear();
              
            if (controle == 1){
              valorLido = analogRead(sensTemp);
              temp = (valorLido * 0.488);
      
                if ((temp > tempMin) && (temp < tempMax)){
                    if (temp > 36.8){
                       oled.clear(); 
                       oled.setFontSize (FONT_SIZE_MEDIUM);
                       oled.setCursor(34, 1);
                       oled.print(temp, 1);
                       oled.print("C");
                       escreveMensagem("Acesso Negado",5, 3);
                       Fbuzzer(5, 300);
                       delay(3000); 
                       oled.clear(); 
                    }else if(temp <= 36.8){
                       oled.setFontSize (FONT_SIZE_MEDIUM);
                       oled.setCursor(34, 1);
                       oled.print(temp, 1);
                       oled.print("C");
                       escreveMensagem("Autorizado",20, 6); 
                       digitalWrite(solenoide, LOW); 
                       Fbuzzer(1, 500);
                       delay(3500);
                       digitalWrite(solenoide, HIGH);  
                       delay(3000);
                       oled.clear();   
                     //  digitalWrite(fotoAcoplador, 1); //abrir tranca
                    }   
                   }else{
                    if ((temp > tempMax) || (temp < tempMin)){
                     escreveMensagem("Error", 44, 3);
                     Fbuzzer(3, 1000); 
                     delay(2000);  
                     oled.clear();
                     escreveMensagem("Tentativas",18 ,3); 
                     escreveMensagem("Finalizadas",16 ,6); 
                     delay(2000);  
                   }  
                  }     
                }  
         }  
       }     
     }
   }else{
      //chama funçao de desenho na tela.
      digitalWrite(led, LOW); 
      
      piscaFrase(2);
      estadoSensObs = digitalRead(sensObs); 
      if (!estadoSensObs == 0){
      gif();
      } 
    }
}



void piscaFrase(int vezes){
  for (int i = 1; i <= vezes; i++){
    estadoSensObs = digitalRead(sensObs);
    if (!estadoSensObs == 0){
          oled.clear(); 
          delay(500);
          oled.setCursor(5, 3);
          oled.setFontSize (FONT_SIZE_MEDIUM);
          oled.println ("INSIRA A MAO");    
          delay(1000);
    }
    }
}

void escreveMensagem(String mensagem, int x, int y){ 
  oled.setCursor(x, y);
  oled.setFontSize(FONT_SIZE_MEDIUM);
  oled.println(mensagem); 
}

void cronometro(int tempoCronometro){
  for(int n = 0;n <= tempoCronometro; n++ ){
    estadoSensObs = digitalRead(sensObs);
     if(!estadoSensObs){ 
        
        oled.clear();
        oled.setFontSize(FONT_SIZE_MEDIUM);
        oled.setCursor(16, 1);
        oled.print("Aguarde "); 
        oled.println(tempoCronometro);
        oled.setCursor(22, 3);
        oled.println("segundos");
        oled.setCursor(53, 6);
        oled.print(n);
        oled.print("s");
        delay(1000);
        if (tempoCronometro == n){
            controle = 1; 
          }
     }    
  }
}
int quantBuzzer; 
int tempo; 
void Fbuzzer(int quantBuzzer, int tempo){
   for (int c = 0; c < quantBuzzer; c++){
    digitalWrite(buzzer, 1);
    delay(tempo);
    digitalWrite(buzzer, 0);
    delay(tempo);  
   } 
  }

void Fservo(){
  while (d < 2){
  servo.write(180);
  delay (500);
  servo.write(90);
  delay(500);
  d++;
  } 
}

void gif()
{
  const unsigned int x = 37, 
                     y = 10,
                     a = 54, 
                     b = 54; 

  

            
  oled.clear(); 
  estadoSensObs = digitalRead(sensObs);
  
  if (!estadoSensObs == 0)
  {
    oled.setCursor(x, y);
    oled.draw(frame1,a, b);
    delay (350);
  }
  
  oled.clear();
  estadoSensObs = digitalRead(sensObs);
  
  if (!estadoSensObs == 0)
  {
    oled.setCursor(x, y);
    oled.draw(frame2,a, b);
    delay (350);
  }
  
  oled.clear();
  estadoSensObs = digitalRead(sensObs);
  
  if (!estadoSensObs == 0)
  { 
    oled.setCursor(x, y);
    oled.draw(frame3,a, b);
    delay (350);
  }
  
  oled.clear();
  estadoSensObs = digitalRead(sensObs);
  
  if (!estadoSensObs == 0)
  {
    oled.setCursor(x, y);
    oled.draw(frame4,a, b);
    delay (350);
  }
  
  oled.clear();
  estadoSensObs = digitalRead(sensObs);
  
  if (!estadoSensObs == 0)
  {
    oled.setCursor(x, y);
    oled.draw(frame5,a, b);
    delay (350); 
  }
  
  oled.clear();
  estadoSensObs = digitalRead(sensObs);
  
  if (!estadoSensObs == 0)
  {
    oled.setCursor(x, y);
    oled.draw(frame6,a, b);
    delay (350); 
  }
  
  oled.clear();
  estadoSensObs = digitalRead(sensObs);
  
  if (!estadoSensObs == 0)
  {
    oled.setCursor(x, y);
    oled.draw(frame7,a, b);
    delay (350); 
  }
    
  oled.clear();
  estadoSensObs = digitalRead(sensObs);
  
  if (!estadoSensObs == 0)
  {
    oled.setCursor(x, y);
    oled.draw(frame8,a, b);
    delay (500);
  }
};
