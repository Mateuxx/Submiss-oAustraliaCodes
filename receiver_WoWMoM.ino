#include <RadioLib.h>


//ESP32 Heltec V2 
SX1276 radio = new Module(18, 26, 14, 35);
unsigned long currentMillis;
unsigned long startMillis = 0;

//Vetores Spreading Factor, BandWidth and Transmission Power
int Vetor_SF[] = {7,8,9,10,11,12}; //6
float Vetor_BW[] = {125,250,500}; //3
int Vetor_PT[] = {10,11,12,13,14,15,16,17,18,19,20}; //11

void SetDefaultParam(){
  
  //Inicializando os parametros default
  radio.setBandwidth(125.0);
  radio.setSpreadingFactor(12);
  radio.setOutputPower(20);
  Serial.println("Parametros default setados");
  
}

void changeParam(int SF, float BW, int PT){
  
  //Troca de parametros
  radio.setBandwidth(BW);
  Serial.println("Trocou para o BandWidth "+String(BW));
  radio.setSpreadingFactor(SF);
  Serial.println("Trocou para o Spreading Factor "+String(SF));
  radio.setOutputPower(PT);
  Serial.println("Trocou para o Transmission Power "+String(PT));
}

void Receiver_HandShake(){
  
  unsigned long period = 5000; // Tempo de espera em milissegundos
  int state = 0;
  String msg_sync = "SYNC_HAND";
  String str_read;
  bool loop = true;

  SetDefaultParam();

  Serial.println("Inicializando HandShake");
  
  while(true){
    state = radio.receive(str_read);
    if(state == RADIOLIB_ERR_NONE && str_read.equals(msg_sync)){
      Serial.println(F("[SX1262] SYNC_HANDR Received!"));
      break;
    }
  }
  
  while(loop){
    
    state = radio.transmit("SYNC+ACK_HAND");
    while(currentMillis - startMillis <= period) {
      currentMillis = millis();  
      String ack_hand = "ACK_HAND";
      state = radio.receive(str_read);
      if(state == RADIOLIB_ERR_NONE && str_read.equals(ack_hand)){
          Serial.println(F("[SX1262] ACK_HAND Received!"));
          loop = false;
          break;
      }
      
    }
  }
  
}

void Receiver_SendPacket(){

  unsigned long period = 5000; // 1 minutos
  String str_read;
  String str_finish = "TERM";

  
  while(true){
    currentMillis = millis();
    if(currentMillis - startMillis >= period){
       startMillis = currentMillis;   
       break;
    }
    int state = radio.receive(str_read);
    if(state == RADIOLIB_ERR_NONE && str_read.equals(str_finish)){
      Serial.print("Mensagem recebida: ");
      Serial.println(str_read);
      startMillis = currentMillis;  
      break;
    }
    else if(state == RADIOLIB_ERR_NONE){
      Serial.print("Mensagem recebida: ");
      Serial.println(str_read);
      state = radio.transmit("ACK_PACKET");
      startMillis = currentMillis;  
    }
    
  }
}



void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  
  if (radio.setFrequency(915.0) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
    while (true);
  }

  Receiver_HandShake();
}

void loop() {

  //Variaveis
  String S = "S";
  String F = "F";

  String str_read;

  SetDefaultParam();
  while(true){
    int state = radio.receive(str_read);
    if(state == RADIOLIB_ERR_NONE && str_read[0] == S && str_read[1] == F){
      Serial.print("Mensagem recebida: ");
      Serial.println(str_read);
      state = radio.transmit("TROCA_ACK");
      break;
    }
  }

  //Parser da mensagem

  //changeParam();
  Receiver_SendPacket();

}
