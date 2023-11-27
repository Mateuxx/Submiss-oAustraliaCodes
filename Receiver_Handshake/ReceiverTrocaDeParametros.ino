#include <RadioLib.h>

#define GPIO_BOTAO 46

SX1262 radio = new Module(8, 14, 12, 13);
char msg[251]; // Declaração do array de caracteres para a mensagem

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  
  if (radio.setFrequency(915.0) == ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
    while (true);
  }

  Serial.println("Preparing message to send.");
  for(int x = 0; x < 250; x++){
    msg[x] = 'a';
  }
  msg[250] = '\0';
  Serial.println("Message prepared!");

  radio.setBandwidth(125.0);
  radio.setSpreadingFactor(12);
  radio.setOutputPower(20);
}

void loop() {
  unsigned long startMillis = millis();
  unsigned long period = 1000; // Tempo de espera em milissegundos
  int state = 0;
  
  while(millis() - startMillis <= period) {
    String str_read;
    String ack_hand = "ACK_HAND"
    state = radio.receive(str_read);
    
    if(state == ERR_NONE){
      Serial.println(F("[SX1262] SYNC_HANDR Received!"));
      break;
    }
  }
  state = radio.transmit("SYNC+ACK_HAND"); 

  darUmtime = startMillis - millis()
  //Tratar caso receba outra ACK
  state = radio.receive(ack_hand);
  if(state == ERR_NONE){
      Serial.println(F("[SX1262] ACK_HAND Received!"));
      break;
    }

}
