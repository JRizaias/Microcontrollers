/***************************************************
READING GPS ublox NEO-6M module SENSOR DATA.
 ***************************************************
 This program makes the raw reading of the data received by the GPS ublox NEO-6M module. Therefore, this is 
 a program for the initial test of the module's operation. In order to obtain 
 information on speed, position, etc., a processing process must be performed on the data received.
 
 Created 02/12/2018
 By [Izaias ALves](https://github.com/JRizaias)
 ****************************************************/


#include <SoftwareSerial.h>
SoftwareSerial PacoteSerial(10, 11); // RX, TX
void setup() {
   PacoteSerial.begin(9600);
   Serial.begin(9600);
   Serial.println("Esperando por Dados do Modulo...");
}

void loop() {
  PacoteSerial.listen();
   if (PacoteSerial.available()){
    Serial.print("\n===============================Dados Recebidos===========================\n");
    //Serial.print("\n");
      while (PacoteSerial.available()) {
         char dados = PacoteSerial.read();
         Serial.print(dados);  
         
      }
   }
}
