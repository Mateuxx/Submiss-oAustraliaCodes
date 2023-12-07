#include <RadioLib.h>
#include "heltec.h"
#include <algorithm>
#define BAND    915E6

//ESP32 Heltec V2
// SX1276 has the following connections:
// NSS pin:   18
// DIO1 pin:  26
// NRST pin:  14
// BUSY pin:  35 
SX1276 radio = new Module(18, 26, 14, 35);
unsigned long currentMillis;
unsigned long startMillis = 0;

int Vetor_SF[] = {7,8,9,10,11,12}; //6
float Vetor_BW[] = {125,250,500}; //3
int Vetor_PT[] = {10,12,14,16,18,20}; //6

int fim = 107;
int inicio = 0;

int valor_SF = 12;
float valor_BW = 125.00;
int valor_PT = 20;

String msg_ult;

struct params_pacote{
  int SF_v;
  float BW_v;
  int PT_v;
  float R_v;
};

params_pacote vetor_Parametros[108];
float vetor_R[108] = {90.53571428571428, 91.30515463917526, 90.7471153846154, 91.51543209876543, 88.75492957746478, 92.43184523809525, 84.43483606557376, 82.44292452830189, 80.26467391304348, 
                      79.88055555555556, 77.67171052631579, 79.03035714285714, 0, 0, 0, 0, 0, 0, 97.0938775510204, 97.38535353535353, 97.53647959183672, 97.97325000000001, 96.81342105263158, 
                      96.60394736842106, 86.673046875, 87.09115384615384, 87.11628787878787, 87.56029411764706, 91.88456790123456, 85.47796610169492, 0, 65.625, 0, 0, 0, 0, 98.2451530612245, 
                      98.988, 99.16125000000001, 99.01025, 99.22925000000001, 99.14, 92.80709876543209, 94.2311046511628, 96.09408602150538, 91.59512987012987, 89.01176470588234, 91.89711538461538, 
                      0, 0, 67.25, 0, 0, 0, 99.34925, 99.0455, 99.23325, 99.31524999999999, 99.19125, 99.03275, 98.74225, 98.7625, 98.69675000000001, 98.7425, 98.55325, 98.5915, 82.68849999999999, 
                      85.30583333333333, 89.75067567567568, 80.29107142857143, 87.82720588235296, 86.0165322580645, 91.09183673469389, 93.53698979591837, 91.22175, 91.8785, 91.58600000000001, 
                      91.327, 90.34100000000001, 90.6985, 90.1965, 90.15858585858585, 90.16900000000001, 90.319, 89.1785, 88.964, 89.26489898989898, 89.281, 89.5345, 89.1235, 90.29848484848485, 
                      90.85883838383839, 90.66377551020409, 90.63341836734693, 91.63575, 91.202, 90.8445, 91.44675000000001, 91.69375, 92.33225, 91.8935, 91.9345, 89.47368421052632, 91.23826530612246, 
                      89.94922680412371, 90.13005319148937, 90.44336734693877, 88.93376288659793};

void bubbleSort(params_pacote arr[], int n) {
  for (int i = 0; i < n-1; i++) {
    for (int j = 0; j < n-i-1; j++) {
      // Troca os elementos se estiverem na ordem errada
      if (arr[j].R_v > arr[j+1].R_v) {
        params_pacote temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }
}

void set_Params(){
  int cont = 0;
  for(int SF = 0; SF < 6; SF++){
    for(int BW = 0; BW < 3; BW++){
      for(int PT = 0; PT < 6; PT++){
        params_pacote pacote;
        pacote.SF_v = Vetor_SF[SF];
        pacote.BW_v = Vetor_BW[BW];
        pacote.PT_v = Vetor_PT[PT];
        pacote.R_v = vetor_R[cont];
        vetor_Parametros[cont] = pacote;
        cont++;
      }
    }
  }
  bubbleSort(vetor_Parametros, 108);
  
}

//Função para deixaros valores default SF=12;BW=125.0;PT=20
void SetDefaultParam(){
  
  //Inicializando os parametros default
  radio.setBandwidth(125.0);
  radio.setSpreadingFactor(12);
  radio.setOutputPower(20);
  //Serial.println("Parametros default setados");
  
}

String* Parser(String msg_read){
  static String valores[3];
    int cont = 0;
    int tam = msg_read.length();
    int aux = 0;
  //Parser da mensagem
    for(int i = 0; i<=tam; i++){
      if(msg_read.charAt(i) == '/'){
        valores[cont] = msg_read.substring(aux,i);
        aux=i+1;   
        cont++;
      }
    }
    return valores;
}

//Função de troca de parâmetros
void changeParam(int SF, float BW, int PT){
  
  //Troca de parametros
  radio.setBandwidth(BW);
  radio.setSpreadingFactor(SF);
  radio.setOutputPower(PT);
  //Serial.println("TRC:SF"+String(SF)+"/BW"+String(BW)+"/PT"+String(PT));
}

//Função de inicialização de transmissão
void Receiver_HandShake(){
  Serial.println("Comecando Handshake");
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  
  Heltec.display->drawString(30, 5, "HandShake");
  Heltec.display->display();
  unsigned long period = 5000; // Tempo de espera em milissegundos
  int state = 0;
  String msg_sync = "SYNC_HAND";
  String str_read;
  bool loop = true;

  SetDefaultParam();
  
  while(true){
    state = radio.receive(str_read);
    if(state == RADIOLIB_ERR_NONE && str_read.equals(msg_sync)){
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
          loop = false;
          break;
      }
    }
  }
  Serial.println("Terminou Handshake");
}

//Função de recebimento de pacote
void Receiver_SendPacket(int SF, float BW,  int PT){
  Serial.println("Comecando Recebimento de pacote");
  unsigned long period = 15000*100; // 1 minutos
  String str_read;
  String str_finish = "TERM";
  bool loop = true;

  //Serial.println("Começando recebimento de pacote");
    Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  
  Heltec.display->drawString(30, 5, "Send Packet");
  Heltec.display->display();
  
  currentMillis = millis();
  startMillis = currentMillis;
  while(loop){
    currentMillis = millis();
    //Serial.println("Esperando mensagens");

    int state = radio.receive(str_read);
    if(state == RADIOLIB_ERR_NONE && str_read.equals(str_finish)){
      //Serial.print("Mensagem recebida: ");
      //Serial.println(str_read);
      startMillis = currentMillis;  
      loop = false;
    }
    else if(state == RADIOLIB_ERR_NONE){
      
      //Serial.print("Mensagem recebida: ");
      //Serial.println(str_read);
      
      Serial.print(String(SF)+";"+String(BW)+";"+String(PT));
      Serial.print(";"+String(radio.getRSSI()));
      Serial.println(";"+String(radio.getSNR()));
      startMillis = currentMillis;
        Heltec.display->clear();
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_16);
      
      Heltec.display->drawString(30, 5, "Packet Receiver");
      Heltec.display->display();  
      
    }
    if(currentMillis - startMillis >= period){
       startMillis = currentMillis;   
       Serial.println("SEMPACTRC");
       loop = false;
    }
      Heltec.display->clear();    
  }
  Serial.println("Terminou recebimento de pacote");
}

int getSF(){
  return valor_SF;
}

float getBW(){
  return valor_BW;
}

int getPT(){
  return valor_PT;
}

//Implementação dos algoritmos
int verificaParam(float vetor_RSSI[],float vetor_SNR[],int contador_perda){
  float somaRSSI = 0;
  float somaSNR = 0;
  int tamanho = sizeof(vetor_RSSI) / sizeof(vetor_RSSI[0]);

  for(int i = 0; i < tamanho;i++){
    somaRSSI+= vetor_RSSI[i];
    somaSNR+= vetor_SNR[i]+20;
  }
  float mediaRSSI = (somaRSSI/tamanho)*-1;
  float mediaSNR = somaSNR/tamanho;
  float porcCt = (contador_perda*100)/20;
  int R = 0.5*mediaRSSI+0.2*mediaSNR+0.3*porcCt;
  Serial.print("Valor do R: ");
  Serial.println(R);
  int j;
  
  for(int i = 0; i< 108;i++){
    if(vetor_Parametros[i].SF_v == valor_SF && vetor_Parametros[i].BW_v == valor_BW && vetor_Parametros[i].PT_v == valor_PT){
      Serial.println("Valor do R do dataBase: "+String(vetor_Parametros[i].R_v));
      j = i;
    }
  }
  
  while(inicio<=fim){
    int intervalo_ideal = R - vetor_Parametros[j].R_v;
    if(intervalo_ideal < 1 && intervalo_ideal > -1){
      Serial.println("0");
      return 0;
    }
    if(intervalo_ideal > 1){
      inicio = j + 1;
      int meio = (inicio+fim)/2;
      valor_SF = vetor_Parametros[meio].SF_v;
      valor_BW = vetor_Parametros[meio].BW_v;
      valor_PT = vetor_Parametros[meio].PT_v;
      Serial.println("+1");
      return 1;    
    }else{
      fim = j-1;
      int meio = (inicio+fim)/2;
      valor_SF = vetor_Parametros[meio].SF_v;
      valor_BW = vetor_Parametros[meio].BW_v;
      valor_PT = vetor_Parametros[meio].PT_v;
      Serial.println("-1");
      return 1;       
    }
    
  }
  return 0;
}

void setup() {

  Serial.begin(115200);
  while(!Serial);
  Heltec.begin(true /*Habilita o Display*/, false /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Habilita debug Serial*/, true /*Habilita o PABOOST*/, BAND /*Frequência BAND*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->drawString(33, 5, "Iniciado");
  Heltec.display->drawString(10, 30, "com Sucesso!");
  Heltec.display->display();

  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
  } else {
    while (true);
  }
  if (radio.setFrequency(915.0) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    while (true);
  }
  SetDefaultParam();
  Receiver_HandShake();
  set_Params();
}

void loop() {

  String str_read;
  int contador = 0;
  unsigned long periodo = 15000*20;
  
  float vetor_RSSI[20];
  float vetor_SNR[20];

  int v_Verifica;
  
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(30, 5, "Recebimento TRC");
  Heltec.display->display();

  while(true){
    int state = radio.receive(str_read);

    if(currentMillis-startMillis >= periodo){
      break;
    }
    if(state == RADIOLIB_ERR_NONE){
    
      vetor_RSSI[contador] = radio.getRSSI();
      vetor_SNR[contador] = radio.getSNR();
      contador++;
     
    }
    if(state == RADIOLIB_ERR_NONE && str_read.equals("TERM")){
      break;
    }    
  }

  SetDefaultParam();
  
  msg_ult = String(getSF())+"/"+String(getBW())+"/"+String(getPT());
  
  if(contador > 10){

    v_Verifica = verificaParam(vetor_RSSI, vetor_SNR, contador);

  }else{
    String msg_volta = "OK_";
    Serial.print("Parametros ideais: ");
    Serial.println(msg_ult);
    msg_volta += msg_ult; 
    radio.transmit(msg_volta);

    String* valores = Parser(msg_ult);
    changeParam(valores[0].toInt(),valores[1].toFloat(),valores[2].toInt());
    while(true){}
  }

  String param_atual = String(getSF())+"/"+String(getBW())+"/"+String(getPT());
  

  if(v_Verifica == 1){
    Serial.print("Trocando para os parâmetros");
    Serial.println(param_atual);
    radio.transmit(param_atual);
    String* valores = Parser(param_atual);
    changeParam(valores[0].toInt(),valores[1].toFloat(),valores[2].toInt());
  }else{
    Serial.print("Parametros ideais: ");
    Serial.println(param_atual);
    radio.transmit("TRC_OK");
    while(true);
  }
  
}
