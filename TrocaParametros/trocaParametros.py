
def Sender_EnvioPacote():
    for i in range(1,11):
        print("[SENDER] envio o pacote numero "+ str(i))
        Receiver_EnvioPacote(i)
    if(i == 10):
        print("[SENDER] Enviou o pacote Terminou")
        print("[RECEIVER] Recebeu o pacote Terminou")


def Receiver_EnvioPacote(i):
    while(True):
        print("[RECEIVER] Recebi o pacote numero "+ str(i))
        print("[RECEIVER] Enviei o pacote ACK "+ str(i))
        break

def trocaParametros(SF_, BW_, PT_,user):
    print(user+" SF foi trocado para "+str(SF_))
    print(user+" BW foi trocado para "+str(BW_))
    print(user+" PT foi trocado para "+str(PT_))
    

def Send_trocaParametros():
    #parametros lora definidos
    LoRaSF = 12
    LoRaPT = 20
    LoRaBW = 125
    
    #vetores de parametros lora
    vetor_SF = [7,8,9,10,11,12]
    vetor_BW = [125,250,500]
    vetor_PT = [10,11,12,13,14,15,16,17,18,19,20]
    
    #Percorrer os vetores para criar a srting de parametros
    for i in vetor_SF:
        for j in vetor_BW:
            for k in vetor_PT:
                string_troca = "SF"+str(i)+"BW"+str(j)+"PT"+str(k)
                print("[SENDER] Enviei o pacote "+string_troca)
                Receive_trocaParametros(string_troca,i,j,k)
                print("[SENDER] Recebi o pacote TROCA_ACK")
                trocaParametros(i,j,k,"[SENDER]")
                Sender_EnvioPacote()
                    
def Receive_trocaParametros(string_troca,i,j,k):
    #parametros lora definidos
    LoRaSF = 12
    LoRaPT = 20
    LoRaBW = 125
    
    
    
    #ele já recebeu uma mensagem
    if(string_troca[0] == "S" and string_troca[1] == "F" ):
        print("[RECEIVER] Recebi o pacote "+string_troca)
        print("[RECEIVER] Enviei o pacote TROCA_ACK")
        trocaParametros(i,j,k,"[RECEIVER]")

def Send_HandShake():
    LoRaSF = 12
    LoRaPT = 20
    LoRaBW = 125

    print("[SENDER] Enviou o pacote SYNC_HAND")
    Receiver_Handshake()
    print("[SENDER] Recebeu o pacote SYNC+ACK_HAND")
    print("[SENDER] Enviou o pacote ACK_HAND")
    print("[RECEIVER] Recebeu o pacote ACK_HAND")  
    Send_trocaParametros()


def Receiver_Handshake():
    LoRaSF = 12
    LoRaPT = 20
    LoRaBW = 125    
    
    print("[RECEIVER] Recebeu o pacote SYNC_HAND")
    print("[RECEIVER] Enviou o pacote  SYNC+ACK_HAND")
    
Send_HandShake()
