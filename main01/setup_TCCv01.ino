//******************************************FUNÇOES MODULO GPS******************************************
void leituraDados(){
  Serial.flush();
  GPSModule.flush();
  while (GPSModule.available() > 0){
    
        GPSModule.read();//ler o primeiro byte armazenado no buffer serial
                    //faz isso ate nao tiver nenhum dado disponivel para
                     //leitura.
    }
    
    }

void dadosGPRMC(){
   if (GPSModule.find("$GPRMC,")) 
   DADOSgprmc = GPSModule.readStringUntil('\n');                  //Na string DADOSgprmc temos todos os dados juntos, separados apenas por virgulas.
    }


void organizaDados(){
    for (int i = 0; i < DADOSgprmc.length(); i++) {                        
                  if (DADOSgprmc.substring(i, i + 1) == ",") {    //Analisaremos caractere por caractere, entre as posiçoes i e i+1, 
                                                                  //buscando indentifcicar e separar os dados brutos.
              
                  nmea[cont]=DADOSgprmc.substring(inicioDado, i); //Separaçao de cada dado bruto existente entre as virgulas, guardando em uma posiçao do vetor.
                  cont++;
                  inicioDado=i+1; // i é a posiçao da virgula atual, o proximo  dado se inicia na proxima posiçao, i+1.
                  }
                  if(i==DADOSgprmc.length()-1){   //Tem a finalidade de captar o ultimo conjunto de caratere que nao termina com virgulas como os demais.
                  nmea[cont]=DADOSgprmc.substring(inicioDado, i);
                  }
  }
  }
  

void printVel(){
  Serial.println("Caracteres VELOCIDADE: ");
  Serial.println(nmea[6]);
  velN=nmea[6].toFloat();
  for ( int p  = 0; p < 15; p++) {
Serial.println("POSICAO:");
Serial.print(p);
Serial.print(" --------> ");

 Serial.println(nmea[p]);

}

  //Serial.println("Covertido VELOCIDADE: ");
 // Serial.println(velN);
  }
    
float retunVelocidade(){
  leituraDados();
  dadosGPRMC();
  
   for ( i = 0; i < DADOSgprmc.length(); i++) {
                  if (DADOSgprmc.substring(i, i + 1) == ",") {
                    contVigula++;
                   if(contVigula==6){
                     vel =DADOSgprmc.substring(i+1, i + 6);
                      velN=vel.toFloat();
                          //Serial.println("VELOCIDADE: ");
                          //Serial.println(vel);
                          //Serial.println(velN);
                    }
                    }
                    }
                    i=0;
                    contVigula=0;

  return velN;
  }
  
/*
  float retunVelocidade(){
  leituraDados();
  dadosGPRMC();
 velN=nmea[6].toFloat();
  return velN;
  }
*/

  //******************************************FUNÇOES MODULO INERCIAL******************************************
  void setupMPU(){
  
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission(); 
   
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
  //********************************************************** 
  
  delay(100);
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings (é o ponto de inicio para)
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
                                //(pede ao escravo no endereço 0b1101000 que faça a leitura de 6 bytes apartir do ponto de inicio, segeuncial)
  while(Wire.available() < 6); //retorna o numero de bytes disponivel devido a solicitaçao anterior.
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

void recordGyroRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 - 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX //Wire.read retorna byte a byte, por isso é possivel a contatenaçao.
  gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processGyroData();
}

void processGyroData() {
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;
}

void printData() {
  Serial.print("Gyro (deg)");
  Serial.print(" X=");
  Serial.print(rotX);
  Serial.print(" Y=");
  Serial.print(rotY);
  Serial.print(" Z=");
  Serial.print(rotZ);
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.println(gForceZ);  
}

void printAnguloPich(){
  double pitch = atan(gForceZ/gForceX) * RAD_TO_DEG;
  Serial.print("\n Angulo pitch : ");
  Serial.print(90-pitch);
}

void printAnguloRoll(){
  double roll = atan(gForceZ/gForceY) * RAD_TO_DEG;
  Serial.print(" Angulo roll: ");
  Serial.print(90-abs(roll));
    // é psoivel saber para qual lado é a rotaçao:      Serial.print(90-roll);
}
  
double returnAnguloPich(){
  double pitch = atan(gForceZ/gForceX) * RAD_TO_DEG;
  return(90-pitch);

}

double returnAnguloRoll(){
  double roll = atan(gForceZ/gForceY) * RAD_TO_DEG;
  return (90-abs(roll));
    // é psoivel saber para qual lado é a rotaçao:      Serial.print(90-roll);
}
