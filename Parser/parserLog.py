import pandas as pd
import matplotlib.pyplot as plt


 
vetor_dicio_info = []


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
    with open('sendLog.txt', 'r') as arquivo:
        linhas = arquivo.readlines()
        for linha in linhas:    
            linha = linha.split(";")
            dicio_info = {}
            dicio_info["Hora"] = linha[0]
            dicio_info["SF"] = linha[1]
            dicio_info["BW"] = linha[2]
            dicio_info["PT"] = linha[3]
            dicio_info["RSSI"] = linha[4]
            dicio_info["SNR"] = linha[5].replace("\n","")
            vetor_dicio_info.append(dicio_info)
    df = pd.DataFrame.from_dict(vetor_dicio_info)
    df.to_csv('logWoWMoM.csv', index=False)
    print(df)
    eixo_x = df.iloc[:,1]
    eixo_y = df.iloc[:,4]
    plt.scatter(eixo_x, eixo_y)
    plt.xlabel('Eixo X')
    plt.ylabel('Eixo Y')
    plt.title('Gráfico de Dispersão')
    plt.show()   


def main():
    # Chamar a função para realizar a operação
    pre_processamento()
    readArquivo()

if __name__ == '__main__':
    main()