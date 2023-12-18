import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib import cm
import numpy as np

from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler


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
    with open('send_experimento_day2.log', 'r') as arquivo:
        dados = []
        dicio_info = {}
        linhas = arquivo.readlines()
        for linha in linhas:
            if(len(linha) > 1):
                dados.append(linha)
    with open("sendLog.txt", 'w') as arquivo_modificado:
        for dado in dados:
            arquivo_modificado.write(dado)

    with open('sendlog.txt', 'r') as arquivo:
        dados = []
        dicio_info = {}
        linhas = arquivo.readlines()
        for linha in linhas:
            if(len(linha) > 1):
                dados.append(linha)
    with open("sendLog2.txt", 'w') as arquivo_modificado:
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
                dicio_info["Distancia"] = 140
                vetor_dicio_info.append(dicio_info)
            else:
                cont+=1
    
    with open('sendLog2.txt', 'r') as arquivo:
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
                dicio_info["Distancia"] = 70
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
Distancia = [70, 140]
pre_processamento()
valor = readArquivo()
valor.insert(6, "PRD", [0 for i in range(len(valor))], True)
#print(valor)
for i in SF:
    for j in BW:
        for k in PT:
            for w in Distancia:
                soma = 0
                media = 0
                NewDf = valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k)]#print(NewDf) 
                #print(len(NewDf))
                if(len(NewDf)==0 or len(NewDf)==1):
                    
                    new_row = {'Hora': None, 'RSSI': None, 'SNR': None,'SF': i, 'BW': j, 'PT': k, 'PRD': 0}
                    valor = pd.concat([valor, pd.DataFrame([new_row])], ignore_index=True)
                    NewDf = valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k)]
                    #print("Tratou a parada!!!   ", len(NewDf))
                    PDR = len(NewDf)
                    valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'PRD'] = PDR
                    valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'R'] = 0
                    
                    print(i, j, k, w)
                else:
                    print(i, j, k, w)
                    #print("Não tratou a parada!!!   ", len(NewDf))
                    PDR = len(NewDf)
                    print("PDR: ",PDR)
                    valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'PRD'] = PDR


                    RSSIva = sum(pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'RSSI']))
                    RSSImi = pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'RSSI']).min()
                    RSSImax = pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'RSSI']).max()
                    

                    
                    print("RSSImi: ", RSSImi)
                    print("RSSImax: ", RSSImax)



                    SNRva = sum(pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'SNR']))
                    SNRmi = pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'SNR']).min()
                    SNRmax = pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'SNR']).max()

                    
                    print("SNRmi: ", SNRmi)
                    print("SNRmax: ", SNRmax)
                    
                    '''
                    #print("Eh a soma ", soma)
                    #print(alcir)
                    mediaRSSI = somaRSSI/ PDR

                    
                    dados = pd.to_numeric(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'SNR'])
                    dados = dados+20
                    
                    somaSNR = sum(dados)

                    mediaSNR = somaSNR/ PDR
                    '''

                    
                    RSSIva = RSSIva/ PDR
                    SNRva = SNRva/PDR


                        
                        #print(PDR)
                    print("RSSIva: ", RSSIva)
                    print("SNRva: ", SNRva)
                    PDR = PDR/120

                    print("PDR porc: ",PDR)

                    RSSInorm = (RSSIva - RSSImi)/ (RSSImax - RSSImi)
                    SNRnorm = (SNRva - SNRmi)/ (SNRmax - SNRmi)
                    print("RSSInorm: ", RSSInorm)
                    print("SNRnorm: ", SNRnorm)

                    valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'RSSInormalizado'] = RSSInorm
                    valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'SNRnormalizado'] = SNRnorm

                    valorR = 0.6*RSSInorm+0.2*SNRnorm+0.2*PDR

                    print("valor do R ", valorR)
                    #print(i, j, k)
                    valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'R'] = valorR
                    alcir.append(valor.loc[(valor['SF'] == i) & (valor['BW'] == j) & (valor['PT'] == k), 'R'].iloc[0])

# Separar os recursos e o alvo
X = valor[['SF', 'BW', 'PT']]
y = valor['R']

# Padronizar os dados
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# Aplicar a Análise de Componentes Principais (PCA)
pca = PCA(n_components=2)
X_pca = pca.fit_transform(X_scaled)

# Criar um novo DataFrame para os resultados da PCA
pca_df = pd.DataFrame(data=X_pca, columns=['PC1', 'PC2'])
pca_df['R'] = y

# Visualizar os resultados
plt.figure(figsize=(10, 6))
plt.scatter(pca_df['PC1'], pca_df['PC2'], c=pca_df['R'], cmap='viridis', edgecolors='k', alpha=0.8)
plt.title('Visualização PCA dos Atributos SF, BW, PT em relação a R')
plt.xlabel('Principal Component 1 (PC1)')
plt.ylabel('Principal Component 2 (PC2)')
plt.colorbar(label='Valor de R')
plt.show()



valor['Concatenation'] = valor['SF'].astype(str) + valor['BW'].astype(str) + valor['PT'].astype(str)

plt.figure(figsize=(12, 12))
sns.scatterplot(x='Concatenation', y='R', data=valor, palette='viridis', alpha=0.8)
plt.title('Concatenation vs R')
plt.xlabel('Concatenation (SF, BW, PT)')
plt.ylabel('R')



plt.xticks(rotation=90)  # Rotacionar os rótulos
plt.show()

#print(valor.head())

df_ordenado = valor.sort_values(by='R')

#print(df_ordenado)

df_ordenado['Concatenation'] = df_ordenado['SF'].astype(str) + df_ordenado['BW'].astype(str) + df_ordenado['PT'].astype(str)

plt.figure(figsize=(12, 12))
sns.scatterplot(x='Concatenation', y='R', data=df_ordenado, palette='viridis', alpha=0.8)
plt.title('Concatenation vs R')
plt.xlabel('Concatenation (SF, BW, PT)')
plt.ylabel('R')
plt.xticks(rotation=90)  # Rotacionar os rótulos
plt.show()

'''
# Scatter plot para SF vs R
plt.figure(figsize=(12, 5))
sns.scatterplot(data=valor, x='SF', y='R')
plt.title('SF vs R')
#plt.show()

# Scatter plot para BW vs R
plt.figure(figsize=(12, 5))
sns.scatterplot(data=valor, x='BW', y='R')
plt.title('BW vs R')
#plt.show()
axes = plt.gca()
axes.set_ylim([0, 2])
# Scatter plot para PT vs R
plt.figure(figsize=(12, 5))
sns.scatterplot(data=valor, x='PT', y='R')
plt.title('PT vs R')
plt.show()
'''
#print(valor)
#print(media)
#print(alcir)
#print(len(alcir))
#print(media)
# print("---------------VAlOR FINAl  DO DATAFRAME-----------------")
# print(valor)