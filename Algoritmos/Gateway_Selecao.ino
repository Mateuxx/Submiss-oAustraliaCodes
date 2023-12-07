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
float vetor_R[108] = {-24.974489795918366, -27.01958762886598, -42.666346153846156, -41.466049382716065, -44.92112676056338, -40.61874999999999, -46.85614754098361, -50.10896226415094, -52.11032608695652, -52.29166666666666, -54.111184210526325, -53.166071428571435, 0, 0, 0, 0, 0, 0, -37.17142857142857, -36.87222222222222, -38.12933673469387, -37.20425, -39.38921052631579, -38.97763157894737, -48.549609375, -48.42807692307693, -47.675378787878785, -46.66029411764706, -43.65246913580248, -50.67457627118644, 0, -66.125, 0, 0, 0, 0, -39.578826530612254, -39.31200000000001, -39.636250000000004, -39.33725, -39.788250000000005, -39.55999999999999, -45.61574074074075, -44.00436046511628, -41.362903225806456, -46.76525974025974, -49.84117647058823, -46.50352564102564, 0, 0, -68.25, 0, 0, 0, -39.428250000000006, -38.7595, -39.14425, -39.282250000000005, -39.06625, -38.729749999999996, -39.765249999999995, -39.772499999999994, -39.705749999999995, -39.7525, -39.34425, -39.43350000000001, -55.1665, -51.2025, -47.60743243243242, -57.583928571428565, -49.19485294117647, -50.89879032258065, -24.89795918367347, -29.41454081632653, -23.580749999999995, -24.5815, -23.969, -23.483000000000004, -23.809000000000005, -24.5815, -23.653499999999994, -24.31616161616162, -23.511000000000003, -23.790999999999997, -24.341499999999996, -23.945999999999998, -25.328535353535354, -24.488999999999997, -24.985499999999995, -24.161499999999997, -23.832828282828284, -24.527525252525255, -24.979081632653063, -24.940561224489798, -24.99675, -24.268, -24.640500000000003, -25.470750000000002, -25.89875, -27.340249999999997, -26.3215, -26.3305, -28.878947368421052, -29.5015306122449, -28.01211340206186, -30.94707446808511, -28.07704081632653, -26.151288659793817};


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
  bubbleSort(parametros, 108);
  
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
}

//Função de recebimento de pacote
void Receiver_SendPacket(int SF, float BW,  int PT){

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
  //Serial.println("Terminou recebimento de pacote");
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
    somaSNR+= vetor_SNR[i];
  }
  float mediaRSSI = somaRSSI/tamanho;
  float mediaSNR = somaSNR/tamanho;
  int R = 0.5*mediaRSSI+0.2*mediaSNR+0.3*contador_perda;

  //Algoritmo-Buscador do parâmetro
  int sub;
  int j;
  for(int i = 0; i< 108;i++){
    if(vetor_Parametros[i].SF_v == valor_SF && vetor_Parametros[i].BW_v == valor_BW && vetor_Parametros[i].PT_v == valor_PT){
      sub = R - vetor_Parametros[i].R_v;
      j = i;
      i = 108;
      if(sub < 1 && sub > -1){
        return 0;
      }
    }
  }
  
  //Algoritmo Incremental
  if(sub > 1){
    valor_SF = vetor_Parametros[i+1].SF_v;
    valor_BW = vetor_Parametros[i+1].BW_v;
    valor_PT = vetor_Parametros[i+1].PT_v;
  }else{
    valor_SF = vetor_Parametros[i-1].SF_v;
    valor_BW = vetor_Parametros[i-1].BW_v;
    valor_PT = vetor_Parametros[i-1].PT_v;
  }
  return 1;
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
    
    msg_volta += msg_ult; 
    radio.transmit(msg_volta);

    String* valores = Parser(msg_ult);
    changeParam(valores[0].toInt(),valores[1].toFloat(),valores[2].toInt());
    while(true){}
  }

  String param_atual = String(getSF())+"/"+String(getBW())+"/"+String(getPT());
  

  if(v_Verifica == 1){
    radio.transmit(param_atual);
    String* valores = Parser(param_atual);
    changeParam(valores[0].toInt(),valores[1].toFloat(),valores[2].toInt());
  }else{
    radio.transmit("TRC_OK");
    while(true);
  }
  
}
