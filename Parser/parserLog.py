import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib import cm
import numpy as np
vetor_dicio_info = []

def plot_correlation(data):
    '''
    plot the correlation's matrix to explore dependency between features
    '''
    fig = plt.figure(figsize=(8,6), dpi=300)
    sns.heatmap(data.corr(), annot=True, fmt=".2f", cmap=cm.coolwarm_r)
    plt.show()
    #fig.savefig('corr.png')

def pre_processamento():
    with open('send_experimento.log', 'r') as arquivo:
        dados = []
        dicio_info = {}
        linhas = arquivo.readlines()
        for linha in linhas:
            if(len(linha) > 1):
                dados.append(linha)
    with open("sendLog.txt", 'w') as arquivo_modificado:
        for dado in dados:
            arquivo_modificado.write(dado)
        
def readArquivo():
    cont = 0
    with open('sendLog.txt', 'r') as arquivo:
        linhas = arquivo.readlines()
        for linha in linhas:    
            dados = linha.split(";")
            
            dicio_info = {}
            if(dados[1] != "SEMPACTRC\n"):
                dicio_info["Hora"] = dados[0]
                dicio_info["SF"] = dados[1]
                dicio_info["BW"] = dados[2]
                dicio_info["PT"] = dados[3]
                dicio_info["RSSI"] = dados[4]
                dicio_info["SNR"] = dados[5].replace("\n","")
                vetor_dicio_info.append(dicio_info)
            else:
                cont+=1
    print("Perdas ao total: ",cont)
    df = pd.DataFrame.from_dict(vetor_dicio_info)
    for linha in df:
        print(linha+"OIEE")
    df.to_csv('logWoWMoM.csv', index=False)
    return df
    '''
    df['PT'] = pd.to_numeric(df['PT'], errors='coerce')
    df['RSSI'] = pd.to_numeric(df['RSSI'], errors='coerce')
    df['SF'] = pd.to_numeric(df['SF'], errors='coerce')
    df['BW'] = pd.to_numeric(df['BW'], errors='coerce')
    fig, axs = plt.subplots(2, 2, figsize=(10, 10))

    sns.boxplot(x='SF', y='RSSI', data=df[['SF', 'RSSI']],
                notch=False, linewidth=1.5, palette = "Set2", width=0.25,
                medianprops={"color": "black"}, ax=axs[0, 0])
    sns.boxplot(x='BW', y='RSSI', data=df[['BW', 'RSSI']],
                notch=False, linewidth=1.5, palette = "Set2", width=0.25,
                medianprops={"color": "black"}, ax=axs[0, 1])
    sns.boxplot(x='PT', y='RSSI', data=df[['PT', 'RSSI']],
                notch=False, linewidth=1.5, palette = "Set2", width=0.25,
                medianprops={"color": "black"}, ax=axs[1, 0])
    axs[1, 1].axis('off')
    plt.tight_layout()
    plt.show()

    '''


alcir = []
soma = 0
PDR = 0
SF =["7","8","9","10","11","12"]   
BW = ["125.00","250.00","500.00"] 
PT = ["10","12","14","16","18","20"]
pre_processamento()
valor = readArquivo()
valor.insert(6, "PRD", [0 for i in range(len(valor))], True)
#print(valor)
for i in SF:
    for j in BW:
        for k in PT:
            soma = 0
            media = 0
            NewDf = valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k)]#print(NewDf) 
            #print(len(NewDf))
            if(len(NewDf)==0):
                new_row = {'Hora': None, 'RSSI': None, 'SNR': None,'SF': i, 'BW': j, 'PT': k, 'PRD': 0}
                valor = pd.concat([valor, pd.DataFrame([new_row])], ignore_index=True)
                NewDf = valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k)]
                #print("Tratou a parada!!!   ", len(NewDf))
                PDR = len(NewDf)
                valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'PRD'] = PDR
                alcir.append(0)
                print(i, j, k)
            else:
                #print("Não tratou a parada!!!   ", len(NewDf))
                PDR = len(NewDf)
                valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'PRD'] = PDR

                #print("sexo" + valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'RSSI'])
                somaRSSI = sum(pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'RSSI']))
                #print("Eh a soma ", soma)
                #print(alcir)
                mediaRSSI = somaRSSI/ PDR 

                somaSNR = sum(pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'SNR']))

                mediaSNR = somaSNR/ PDR
                valorR = 0.5*mediaRSSI+0.2*mediaSNR+0.3*PDR
                print(i, j, k)
                valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'R'] = valorR
                alcir.append(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'R'].iloc[0])

#print(valor)
#print(media)
print(alcir)
print(len(alcir))
#print(media)
# print("---------------VAlOR FINAl  DO DATAFRAME-----------------")
# print(valor)