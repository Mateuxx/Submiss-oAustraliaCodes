#include <RadioLib.h>

//ESP32 Heltec V2 
SX1276 radio = new Module(18, 26, 14, 35);
unsigned long currentMillis;
unsigned long startMillis = 0;

//Vetores Spreading Factor, BandWidth and Transmission Power
int Vetor_SF[] = {7,8,9,10,11,12}; //6
float Vetor_BW[] = {125,250,500}; //3
int Vetor_PT[] = {10,11,12,13,14,15,16,17,18,19,20}; //11


bool startsWith(const String &str, const String &prefix) {
    return str.substring(0, prefix.length()) == prefix;
}


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

  unsigned long period = 15000*50; // 1 minutos
  String str_read;
  String str_finish = "TERM";
  bool loop = true;

  Serial.println("Começando recebimento de pacote");
  
  currentMillis = millis();
  startMillis = currentMillis;
  while(loop){
    currentMillis = millis();
    Serial.println("Esperando mensagens");

    int state = radio.receive(str_read);
    if(state == RADIOLIB_ERR_NONE && str_read.equals(str_finish)){
      Serial.print("Mensagem recebida: ");
      Serial.println(str_read);
      startMillis = currentMillis;  
      loop = false;
    }
    else if(state == RADIOLIB_ERR_NONE){
      
      Serial.print("Mensagem recebida: ");
      Serial.println(str_read);
      delay(2000);
      state = radio.transmit("ACK_PACKET");
      //radio.finishTransmit();
      Serial.println("Msg ENVIADA");
      radio.startReceive(); 
      startMillis = currentMillis;  
      
    }
    else{
      Serial.println(state);
    }
    if(currentMillis - startMillis >= period){
       startMillis = currentMillis;   
       Serial.println("Sem comunicação na troca");
       loop = false;
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
    Serial.println("Esperando mensagem para troca de parametro");
    if(state == RADIOLIB_ERR_NONE){
      Serial.print("Mensagem recebida: ");
      Serial.println(str_read);
      state = radio.transmit("TROCA_ACK");
      break;
    }
  }

    String valores[3];
    int cont = 0;
    int tam = str_read.length();
    int aux = 0;
  //Parser da mensagem
    for(int i = 0; i<=tam; i++){
    if(str_read.charAt(i) == '/'){
      valores[cont] = str_read.substring(aux,i);
      aux=i+1;   
///      str_read = str_read.substring(i+1,tam);
      cont++;
    }
        
  }
  changeParam(valores[0].toInt(),valores[1].toFloat(),valores[2].toInt());
  Receiver_SendPacket();

}
