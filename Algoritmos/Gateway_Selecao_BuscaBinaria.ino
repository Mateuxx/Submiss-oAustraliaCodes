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

int inicio = 0;
int fim = 107;

String msg_ult = "12/125.00/20/";

struct params_pacote{
  int SF_v;
  float BW_v;
  int PT_v;
  float R_v;
};

params_pacote vetor_Parametros[108];
float vetor_R[108] = {0.5970420957703396, 0.5973553304138925, 0.516460507867269, 0.5303664787693307, 0.48100427941660706, 0.5457165800475018, 0.4121464841469779, 0.3729636846614985, 0.3316721588767846, 0.3310470986231496, 0.28573231172382363, 0.3164234968962133, 0.05877853600618842, 0.06355987239375645, 0.0590231945935249, 0.059612653886053885, 0.05873656026816497, 0.056507048925720935, 0.61966439635179, 0.6243320108036379, 0.6196131259860613, 0.6293888382514906, 0.6050230345121389, 0.6045609572449733, 0.4324140746648437, 0.4379966691017298, 0.44146482216999505, 0.4519283872103742, 0.5179778888792377, 0.40750593106551497, 0.06898541642930382, 0.07700203996375812, 0.06832402871140479, 0.0684732824427481, 0.06442007519653636, 0.06872906460066082, 0.621650616113378, 0.6313909460331929, 0.6322362424518628, 0.6320480612206145, 0.6327250199384756, 0.6328525312369451, 0.5198471985172769, 0.5442496146661191, 0.5796339840351037, 0.4997568210728837, 0.4537057598889659, 0.5049506014095281, 0.07606585393642475, 0.07218468725076903, 0.08344505784747323, 0.07369260567392046, 0.07682351600774744, 0.07607382932664918, 0.6376796361702937, 0.6372614029091185, 0.6373336561467471, 0.6378602217918043, 0.6372164938665451, 0.6369684972086135, 0.6207163609433748, 0.6211364931069843, 0.620044339371843, 0.6211309862899244, 0.6201948273897687, 0.6203478789259809, 0.3417521444033921, 0.39332872849492995, 0.46334588225027207, 0.30115871026546653, 0.4325711052647932, 0.402874808604529, 0.6097046208986121, 0.61811711271563, 0.6171116744521666, 0.6212789107895637, 0.6205516311571911, 0.6196963655007406, 0.5957785499981011, 0.5958630511564316, 0.5943429797577, 0.5908395845520618, 0.5949098021343663, 0.5956591090349778, 0.5660086969731495, 0.5655401427974631, 0.5627657322470973, 0.5668037674224298, 0.5673510311040219, 0.5664539895940146, 0.5997272138888596, 0.604150635302543, 0.6000123010256004, 0.5996566913137765, 0.6123596711100984, 0.6110145835706962, 0.598875280088109, 0.6030679617181269, 0.6041892636056359, 0.60368871292393, 0.6038016026736546, 0.6046287645740761, 0.5496362776228383, 0.5698937129593332, 0.555498658327211, 0.5431381547306364, 0.5649022387094158, 0.5556548486769518};

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
  //Serial.println("Comecando Handshake");
  /*
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  
  Heltec.display->drawString(30, 5, "HandShake");
  Heltec.display->display();
  */
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
  //Serial.println("Terminou Handshake");
}

//Função de recebimento de pacote
void Receiver_SendPacket(int SF, float BW,  int PT){
  Serial.println("Comecando Recebimento de pacote");
  unsigned long period = 15000*100; // 1 minutos
  String str_read;
  String str_finish = "TERM";
  bool loop = true;

  //Serial.println("Começando recebimento de pacote");
  /*
    Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  
  Heltec.display->drawString(30, 5, "Send Packet");
  Heltec.display->display();
  */
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
      /*
        Heltec.display->clear();
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_16);
      
      Heltec.display->drawString(30, 5, "Packet Receiver");
      Heltec.display->display();  
      */
    }
    if(currentMillis - startMillis >= period){
       startMillis = currentMillis;   
       Serial.println("SEMPACTRC");
       loop = false;
    }
      //Heltec.display->clear();    
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

// Função para encontrar o valor máximo em um vetor
float encontraMaximo(float vetor[], int tamanho) {
    float maximo = vetor[0];  // Inicializa o máximo com o primeiro elemento do vetor

    for (int i = 1; i < tamanho; i++) {
        if (vetor[i] > maximo) {
            maximo = vetor[i];
        }
    }

    return maximo;
}

// Função para encontrar o valor mínimo em um vetor
float encontraMinimo(float vetor[], int tamanho) {
    float minimo = vetor[0];  // Inicializa o mínimo com o primeiro elemento do vetor

    for (int i = 1; i < tamanho; i++) {
        if (vetor[i] < minimo) {
            minimo = vetor[i];
        }
    }

    return minimo;
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
  float mediaRSSI = (somaRSSI/tamanho);
  float mediaSNR = somaSNR/tamanho;

  int RSSImi = encontraMinimo(vetor_RSSI,20);
  int RSSImax = encontraMaximo(vetor_RSSI,20);

  int SNRmi = encontraMinimo(vetor_SNR,20);
  int SNRmax = encontraMaximo(vetor_SNR,20);

  float RSSInorm = (mediaRSSI - RSSImi)/ (RSSImax - RSSImi);
  float SNRnorm = (mediaSNR - SNRmi)/ (SNRmax - SNRmi);
  
  float porcCt = (contador_perda*100)/20;
  porcCt = porcCt/100;
  
  float R = 0.2*((-1)*RSSInorm)+0.2*SNRnorm+0.6*porcCt;
  //Serial.println("RSSI: "+String(mediaRSSI));
  //Serial.println("SNR: "+String(mediaSNR));
  //Serial.println("R atual: "+String(R));
  
  //Serial.print("R: ");
  //Serial.println(R);
  int j;
  
  for(int i = 0; i< 108;i++){
    if(vetor_Parametros[i].SF_v == valor_SF && vetor_Parametros[i].BW_v == valor_BW && vetor_Parametros[i].PT_v == valor_PT){
      //Serial.println("Valor do R do dataBase: "+String(vetor_Parametros[i].R_v));
      j = i;
    }
  }
  
  while(inicio<=fim){
    float intervalo_ideal = R - vetor_Parametros[j].R_v;
    if(intervalo_ideal < 0.1 && intervalo_ideal > -0.1){
      //Serial.println("0");
      return 0;
    }
    if(intervalo_ideal < 0){
      inicio = j + 1;
      int meio =  (int)(inicio+fim)/2;
      valor_SF = vetor_Parametros[meio].SF_v;
      valor_BW = vetor_Parametros[meio].BW_v;
      valor_PT = vetor_Parametros[meio].PT_v;
      Serial.println("Direita");
      return 1;    
    }else{
      fim = j-1;
      int meio = (int)(inicio+fim)/2;
      valor_SF = vetor_Parametros[meio].SF_v;
      valor_BW = vetor_Parametros[meio].BW_v;
      valor_PT = vetor_Parametros[meio].PT_v;
      Serial.println("Esquerda");
      return 1;       
    }
    
  }
  //Serial.println("faz algo");
  return 0;
}

void setup() {

  Serial.begin(115200);
  while(!Serial);
  
  
  //Heltec.begin(true /*Habilita o Display*/, false /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Habilita debug Serial*/, true /*Habilita o PABOOST*/, BAND /*Frequência BAND*/);
  /*
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->drawString(33, 5, "Iniciado");
  Heltec.display->drawString(10, 30, "com Sucesso!");
  Heltec.display->display();

  */

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
  Serial.println("COM_VER");
}

void loop() {

  String str_read;
  int contador = 0;
  unsigned long period = 1000*20;
  
  float vetor_RSSI[20];
  float vetor_SNR[20];

  int v_Verifica;
  /*
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(30, 5, "Recebimento TRC");
  Heltec.display->display();
  */
  float startMillis = millis();
  float currentMillis = 0;
  
  while(currentMillis - startMillis <= period){
    currentMillis = millis();
    while(true){
      int state = radio.receive(str_read);
  
      if(currentMillis-startMillis >= period){
        break;
      }
      if(state == RADIOLIB_ERR_NONE && str_read.equals("TERM")){
        //Serial.println("Recebeu TERM");
        break;
      }
      if(state == RADIOLIB_ERR_NONE){
      
        vetor_RSSI[contador] = radio.getRSSI();
        vetor_SNR[contador] = radio.getSNR();
        contador++;
        Serial.println(str_read);
       
      }
          
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
    Serial.println("TERM_VER");
    Serial.print("PARM_IDEAIS: ");
    Serial.println(msg_ult);
    String valor;
    while(true){
      int state = radio.receive(valor);
  
      if(currentMillis-startMillis >= period){
        break;
      }
      if(state == RADIOLIB_ERR_NONE && str_read.equals("TERM")){
        //Serial.println("Recebeu TERM");
        break;
      }
      if(state == RADIOLIB_ERR_NONE){
      
        Serial.println(valor);
       
      }
          
    }
    while(true){}
  }

  String param_atual = String(getSF())+"/"+String(getBW())+"/"+String(getPT())+"/";
  
  Serial.print("Trocando para os parâmetros");
    Serial.println(param_atual);
  if(v_Verifica == 1){
    
    radio.transmit(param_atual);
    String* valores = Parser(param_atual);
    changeParam(valores[0].toInt(),valores[1].toFloat(),valores[2].toInt());
    
  }else{
    
    radio.transmit("TRC_OK");
    Serial.println("TERM_VER");
    Serial.print("PARM_IDEAIS: ");
    Serial.println(param_atual);
    String valor;
    startMillis = millis();
    while(true){
      currentMillis = millis();
      int state = radio.receive(valor);
      
      if(currentMillis-startMillis >= period){
        break;
      }
      if(state == RADIOLIB_ERR_NONE && valor.equals("TERM")){
        //Serial.println("Recebeu TERM");
        break;
      }
      if(state == RADIOLIB_ERR_NONE){
      
        Serial.println(valor);
       
      }
          
    }
    while(true);
  }
  
}
