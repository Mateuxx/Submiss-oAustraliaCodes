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
    with open('sendlog.txt', 'r') as arquivo:
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

    df['PT'] = pd.to_numeric(df['PT'], errors='coerce')
    df['SNR'] = pd.to_numeric(df['SNR'], errors='coerce')
    df['SF'] = pd.to_numeric(df['SF'], errors='coerce')
    df['BW'] = pd.to_numeric(df['BW'], errors='coerce')
    fig, axs = plt.subplots(2, 2, figsize=(10, 10))

    sns.boxplot(x='SF', y='SNR', data=df[['SF', 'SNR']],
                notch=False, linewidth=1.5, palette = "Set2", width=0.25,
                medianprops={"color": "black"}, ax=axs[0, 0])
    sns.boxplot(x='BW', y='SNR', data=df[['BW', 'SNR']],
                notch=False, linewidth=1.5, palette = "Set2", width=0.25,
                medianprops={"color": "black"}, ax=axs[0, 1])
    sns.boxplot(x='PT', y='SNR', data=df[['PT', 'SNR']],
                notch=False, linewidth=1.5, palette = "Set2", width=0.25,
                medianprops={"color": "black"}, ax=axs[1, 0])
    axs[1, 1].axis('off')
    plt.tight_layout()
    plt.show()



    
pre_processamento()
readArquivo()
   

