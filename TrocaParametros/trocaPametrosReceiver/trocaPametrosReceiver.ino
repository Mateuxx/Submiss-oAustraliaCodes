#include <RadioLib.h>
#include <string.h>
#include <Regexp.h>



#define GPIO_BOTAO 46

SX1276 radio = new Module(18, 26, 14, 35);
char msg[251]; // Declaração do array de caracteres para a mensagem

bool startsWith(const String &str, const String &prefix) {
    return str.substring(0, prefix.length()) == prefix;
}


void parser(String parametros){

  MatchState ms;

  ms.Target(parametros.c_str());

    while (regex.Match(ms)) {
      for (uint8_t i = 0; i < ms.level; i++) {
        String match = String(ms.GetCapture(ms.level - 1 - i));
        Serial.println(match);
      }
    }
}




void Receiver_TrocaParametros(){
  unsigned long period = 5000; // Tempo de espera em milissegundos
  int state = 0;
  String msg_sync = "SYNC_HAND";
  String str_read;
  bool loop = true;

  Serial.println("Inicializando Comunicação");
  
  while(true){
    state = radio.receive(str_read);
    if(state == RADIOLIB_ERR_NONE &&  startsWith(str_read,"SF")){
      Serial.println(F("[SX1262] SYNC_HANDR Received!"));
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Regexp regex("\\d+\\.?\\d*");


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

  // Serial.println("Preparing message to send.");
  // for(int x = 0; x < 250; x++){
  //   msg[x] = 'a';
  // }
  // msg[250] = '\0';
  // Serial.println("Message prepared!");

  radio.setBandwidth(125.0);
  radio.setSpreadingFactor(12);
  radio.setOutputPower(20);

}

void loop() {
 
}
