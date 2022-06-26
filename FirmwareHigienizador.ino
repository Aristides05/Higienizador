/*BIBLIOTECAS*/
/*==================================================================================*/
#include <Wire.h>
#include <WiFi.h>
#include "SSD1306.h"
#include <Adafruit_MLX90614.h>
/*==================================================================================*/

/*CONSTANTES ACESSO WIFI*/
/*==================================================================================*/
const char *ssid = "Allan Costa";
const char *password = "1205032003"; 
/*==================================================================================*/

/*OBJETOS*/
/*==================================================================================*/
SSD1306 display(0x3c, 21, 22); 
Adafruit_MLX90614 sensorTemp = Adafruit_MLX90614();
/*==================================================================================*/

/*MAPEAMENTO*/
/*==================================================================================*/                        
#define sensObs 39  //ENTRADA
#define solenoide 13  //SAIDA
#define buzzer 14  //SAIDA
#define led 26  //SAIDA
#define motor1 33  //SAIDA
#define motor2 35 //SAIDA
#define ledESP 2  //SAIDA
/*==================================================================================*/

/*PROTOTIPAGEM*/
/*==================================================================================*/                           
//void gif(); 
void piscaFrase(int vezes);
void escreveMensagem(char mensagem, int x, int y);  
void Fbuzzer(int quantBuzzer, int tempo);
void TestaTemperatura();
void medeTemperatura();
void acionaMotor();
void VerificaWifiSetup();
void VerificaWifiLoop();
void GestaoServidor(); 
/*==================================================================================*/

/*VARIAVEIS CALCULO DA TEMPERATURA*/
/*==================================================================================*/
byte amostra = 4;                   // numero de amostras

float temp = 0.0;
/*==================================================================================*/

/*CONTROLE SERVIDOR*/
/*==================================================================================*/ 
WiFiServer server(80);
/*==================================================================================*/ 

/*VARIAVEIS*/
/*==================================================================================*/ 
float valorLido = 0; 

byte vezes;
     
bool temperaturaFora = false,
     PermAdmin = true;  
     
unsigned int quantBuzzer, 
             tempo, 
             pessoasDentro = 0; 

byte tempMin = 28,
     tempMax = 40;
/*==================================================================================*/

/*FUNÇÃO SETUP*/
/*==================================================================================*/
void setup() 
{
  sensorTemp.begin();
  WiFi.begin(ssid, password); 
  display.init(); 
  Serial.begin(115200);
  pinMode(sensObs, INPUT);
  pinMode(solenoide, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(ledESP, OUTPUT);
  digitalWrite(11, HIGH); 
  VerificaWifiSetup(); 
  server.begin();
} //END SETUP
/*==================================================================================*/

/*FUNÇÃO LOOP*/
/*==================================================================================*/
void loop() 
{ 
  VerificaWifiLoop(); 
  GestaoServidor();
  if(PermAdmin)
  {
    if(!digitalRead(sensObs))                       //verifica se alguem colocou a mão.
    {                                               //Como bloco verdadeiro temos que alguem colocou a mão                         
      digitalWrite(led, HIGH); 
      acionaMotor();                                   
      delay(250); 
      display.clear();
      medeTemperatura();  
      TestaTemperatura(); 
      display.clear();                                                
    }else
    {
        //chama funçao de desenho na tela.
      digitalWrite(led, LOW);   
      piscaFrase(2); 
      if(!digitalRead(sensObs) == 0)
      {
        //gif();
      }  //END IF 
    }  //END ELSE
  }else
  {
    //ESCREver  "local cheio"
  }//END IF ELSE PERM
}  //END LOOP
/*==================================================================================*/

/*FUNÇÃO PISCA FRASE*/
/*==================================================================================*/
void piscaFrase(int vezes)
{
  vezes = 0;
  for (int i = 1; i <= vezes; i++)
  {
    if(!digitalRead(sensObs) == 0)
    {
          display.clear(); 
          delay(500);
          display.setFont(ArialMT_Plain_10);  
          display.drawString(5, 3, "INSIRA A MAO");
          display.display(); 
          delay(1000);
    }  //END IF
  }  //END FOR
}  //END PISCA FRASE
/*==================================================================================*/

/*FUNÇÃO ESCREVE MENSAGEM*/
/*==================================================================================*/
void escreveMensagem(String mensagem, int x, int y){ 
  display.setFont(ArialMT_Plain_10);
  display.drawString(x, y, mensagem);
  display.display();
}  //END ESCREVE MENSAGEM
/*==================================================================================*/

/*FUNÇÃO BUZZER*/ 
/*==================================================================================*/
void Fbuzzer(int quantBuzzer, int tempo)
{
   for(int c = 0; c < quantBuzzer; c++)
   {
    digitalWrite(buzzer, 1);
    delay(tempo);
    digitalWrite(buzzer, 0);
    delay(tempo);  
   }  //END FOR 
}  //END BUZZER
/*==================================================================================*/

/*FUNÇÃO VERIFICAR TEMPERATURA*/ 
/*==================================================================================*/
void TestaTemperatura()
{
    if((temp > tempMin) && (temp < tempMax))
    {
        String tempString = String (temp, 2);
        if(temp > 36.8)
        {
            display.clear(); 
            display.setFont(ArialMT_Plain_10);
            display.drawString(34, 1, tempString);
            display.display();
            escreveMensagem("Acesso Negado",5, 3);
            Fbuzzer(5, 300);
            delay(3000); 
            display.clear(); 
        }else if(temp <= 36.8)
        {
            display.clear(); 
            display.setFont(ArialMT_Plain_10);
            display.drawString(34, 1, tempString);
            escreveMensagem("Autorizado",20, 6);
            digitalWrite(solenoide, LOW);
            Fbuzzer(1, 500);  
            delay(3500);
            digitalWrite(solenoide, HIGH);   
            Serial.println(temp);
            delay(3000);
            display.clear(); 
            pessoasDentro ++; 
        }  //END ELSE IF 
    }else
    {
      display.setFont(ArialMT_Plain_10);
      display.drawString(12, 4, "Temperatura Anormal"); 
      display.display();
    }//END ELSE
}  //END TESTA TEMPERATURA
/*==================================================================================*/

/*FUNÇÃO MEDE TEMPERATURA*/
/*==================================================================================*/
void medeTemperatura()
{
    double somatoria = 0; 
    for(vezes = 0; vezes <= amostra; vezes++)
    {
      somatoria += sensorTemp.readObjectTempC();
      delay(50); 
    }  //END FOR
    temp = somatoria / amostra;  
}  //END MEDE TEMPERATURA
/*==================================================================================*/

/*FUNÇÃO ACIONA MOTOR*/
/*==================================================================================*/
void acionaMotor()
{
  digitalWrite(motor1, 1);
  digitalWrite(motor2, 1); 
  delay(2000);
  digitalWrite(motor1, 0);
  digitalWrite(motor2, 0); 
}
/*==================================================================================*/

/*FUNÇÃO VERIFICA WIFI SETUP*/
/*==================================================================================*/
void VerificaWifiSetup()
{
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("."); 
    delay(300); 
  }
  Serial.println("");
  Serial.print("Conectado a rede: "); 
  Serial.println(ssid);
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}
/*==================================================================================*/

/*FUNÇÃO VERIFICA WIFI LOOP*/ 
/*==================================================================================*/
void VerificaWifiLoop()
{
  while(WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password); 
    delay(200); 
  }
}
/*==================================================================================*/

/*FUNÇÃO GESTÃO SERVIDOR*/
/*==================================================================================*/
void GestaoServidor()
{
  WiFiClient client = server.available();

  if(client)
  {
    String currentLine = ""; 

    while(client.connected())
    {
      if(client.available())
      {
        char c = client.read(); 
        if(c == '\n')
        {
          if(currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            break;
          }else
          {
            currentLine = ""; 
          } 
        }else if(c != '\r')
        {
          currentLine += c; 
        }
        if(currentLine.endsWith("GET /H"))
        {
          PermAdmin = !PermAdmin;
          Serial.println(""); 
          Serial.print("Permissão Admin: "); 
          Serial.println(PermAdmin);
          digitalWrite(2, PermAdmin);
        }
      }  //END IF CLIENT AVAILABLE
    }  //END WHILE CLIENT CONNECTED
  }  //END IF CLIENT
}
/*==================================================================================*/
