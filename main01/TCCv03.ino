 #include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial GPSModule(10, 11); // RX, TX  //determinaçao de um objeto do tipo, semelhante, igual a serial .
TinyGPS gps1;                     //Objeto que vai receber os dados do gps.

//**********************************VERIAVEIS DO MODULO GPS**********************************
int inicioDado = 0;               //Variavel resposavel por amazenaar o posiçao de "inicio do dado" para o comando .substring(INICIO, FIM);
int cont=0;                       //Organiza os posiçoes do vetor nmea[cont] dentro da funçao organizaDados();
int i=0;                          //Var. de controle para leitura dos dados dentro da string DADOSgprmc.
float velN=0;
String DADOSgprmc="nada ainda";   //resposavel por guardar o trecho de dados no formato GPRMC no qual iremos procurar 
                                  // pela velocidade.
String nmea[15];                 //Responsavel por armazenar em cada posiçao do vetor, um respectivo tipo dado em caractere(ex: altura, velocidade etc)
int contVigula=0;
String vel="0";                 //Armazena a velocidade que esta em nmea[6] no formato de string.

void leituraDados();            //Funçao responsavel armazenar todos os diferentes formatos de dados recebidos do satelite 
                                //em um unico texto de caracteres atraves do objeto GPSModule.
void organizaDados();           //Funçao resposavel por separar cada dado existente na string DADOSgprmc e amazena-lo no array String nmea[15]. 
void dadosGPRMC();              //Funçao responsavel por encontrar separar os dados no fomatao GPRMC dentro do objeto GPSModule
                                //atraves das funçoes .find e .readStringUntil e armazenar-lo separadamente na string DADOSgprmc.
void printVel();                //Funçao resposavel por printar a velocidade em nos formatos caractere e Valor numerico se necessario.
float retunVelocidade();        //Funçao responsavel por retornar  a velocidade  no formato numerico.
//**********************************VERIAVEIS DO MODULO INERCIAL**********************************
long accelX, accelY, accelZ;    //Armazena os dados brutos ja lidos e ja contatenados  dos Acelerometros na unidade LSB/g pela funçao recordAccelRegisters(). 
float gForceX, gForceY, gForceZ;//Armazena os resultados da funçao processAccelData(), estes ja estao na unidade da gravidade g, com valor maximo de 2g.*  

long gyroX, gyroY, gyroZ;       //Armazena os dados brutos ja lidos e ja contatenados  dos Giroscopoios na unidade LSB/g pela funçao recordGyroRegisters().
float rotX, rotY, rotZ;         //Armazena os resultados da funçao processGyroData(), estes ja estao na unidade da gravidade graus.

const long interval = 1000;
unsigned long TempoVelho = 0;
unsigned long  deltaTime=0;
unsigned long TempoAtual= millis();
float velocidade; 
bool estolLigado=false;
bool avisoEstolLigado=false;


void setup(){
pinMode(5, OUTPUT);
pinMode(6, OUTPUT);
pinMode(7, OUTPUT);
Serial.begin(9600);             //Configura a taxa de transferência em bits por segundo (entre escavo e pc) (baud rate) para transmissão serial para 9600.
GPSModule.begin(9600);          //incio de comunicaçao serial entre o arduino e modulo gps na velocidade de transferencia de dados de 9600.

Wire.begin();                   //inicia a comunicaçao com dispositivos via I2C.                           
setupMPU();                     //Executa a funçao setupMPU() responsavel por setar os registradores do modulo mpu6050.

}


void loop(){

//UNIDADE INERCIAL
recordAccelRegisters();     //Ler dados os via I2C do acelerometro, concatena, e faz o precesamento de dados atraves da funaçao processAccelData().
recordGyroRegisters();      //Ler dados os via I2C do Gyroscopio, concatena, e faz o precesamento de dados atraves da funaçao processGyroData().
                          
                        
//Serial.println("Angulo pitch: ");
double pitchVar=returnAnguloPich();

//UNIDADE DE GPS
bool recebido = false;


 while (GPSModule.available()) { //Comando de verificaçao se ha infomaçoes disponiveis.
      char cIn = GPSModule.read(); //comando para a leitura do texto com a as informaçoes
      recebido = gps1.encode(cIn);// O caratere cIn é enviado para  o objeto gps1, declarado

             if (recebido) {
             velocidade = gps1.f_speed_kmph();   //km/h
          } 
  }

//-----------------------------------amostragem de dados-------------------
TempoAtual=millis();
deltaTime=TempoAtual- TempoVelho;
if(deltaTime>=interval){

  Serial.println("----------------------------------------");
  Serial.print("Velocidade (km/h): ");
  Serial.println(velocidade);

  Serial.print("Angulo pitch: ");
  Serial.println(pitchVar);
  TempoVelho = millis();
  
  
  //-----------------------------SITUAÇÃO DE ESTOL-----------------------------
            if (pitchVar>=20 && pitchVar<=30 ){
              if(estolLigado ==false ){
                digitalWrite(5,HIGH);
                delay(500);
                digitalWrite(6,HIGH);
                delay(500);
                digitalWrite(6,LOW);
                estolLigado=true;
              }
              else{
                estolLigado=true;
                }
}


//------------------------------SITUAÇÃO DE PRE-ESTOL-----------------------------
            else if( pitchVar>30){
                    
                    if(avisoEstolLigado ==false ){
                    digitalWrite(5,HIGH);
                    delay(500);
                    digitalWrite(6,HIGH);
                    delay(500);
                    digitalWrite(6,LOW);
                    avisoEstolLigado=true;
                  }
                  else{
                    avisoEstolLigado=true;
                    }
            } 

//-----------------------------SITUAÇÃO NORMAL-----------------------------
            else{
            digitalWrite(5,LOW);
            digitalWrite(6,LOW);
            digitalWrite(7,LOW);
            avisoEstolLigado = false;
            estolLigado = false;
            }

  }
  
  
//-------------------------------------------------------------------------

}
         
