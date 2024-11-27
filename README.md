# Comunicação
## TCP e UDP
- Cliente inicia conexão com servidor
- Cliente envia o nome do arquivo que deseja baixar
- Servidor abre o arquivo
- Servidor envia mensagem com tamanho do arquivo, tamanho do buffer de transmissão e tamanho de janela no caso do UDP.
- Cliente recebe dados de transmissão, aloca buffer e se prepara para receber dados
- Servidor envia dados do arquivo para o cliente
- Servidor encerra conexão
- Cliente fecha arquivo

Tanto o servidor TCP quanto o servidor UDP funcionam de forma iterativa ou paralela, sendo que o paralelo
cria uma thread para fazer o envio do arquivo solicitado pelo cliente.

# Testes

Os mesmos testes para servidor iterativo serão feitos para servidor paralelo.

Dois tipos de teste: 
- Rede local 
- Passando pela internet (Usando LAN virtual/radmin)

Cada teste é feito com base no número de clientes:
- O número de clientes começa em 1 e vai dobrando até 64.
- Cada cliente faz uma transferência de arquivos de 1MB, 10MB, 100MB e 1GB
- Os dados de cada teste são salvos em um arquivo CSV

Para cada teste, será analisado as métricas:
- Perda de pacotes
- Tempo de download
- sha256sum do arquivo baixado

## Estrutura de diretórios
Cada teste vai gerar logs salvos no diretório `logs/` com a seguinte estrutura:

```
logs/
    server/
        rede_externa/
            TCP/
            UDP/
        rede_local/
            TCP/
                iter_8192.txt
                par_4096.txt
            UDP/
                iter_8192_8.txt
                par_4096_15.txt
    client/
        rede_externa/
            TCP/
            UDP/
        rede_local/
            TCP/
                iter_1.csv
                iter_2.csv
                par_1.csv
                par_2.csv
            UDP/ 
                iter_1.csv
                iter_2.csv
                par_1.csv
                par_2.csv
```

### Logs server:
Onde `iter_8192.txt` é o log do servidor TCP iterativo com tamanho de buffer igual a 8192 bytes. Enquanto
`par_N.txt` é o log do servidor TCP paralelo com tamanho de buffer N bytes.

Para o UDP, é a mesma coisa com a adição do tamanho da janela no final do nome. Por exemplo,
`iter_N_M.txt` é o servidor UDP iterativo com tamanho de buffer igual a N bytes e tamanho de janela M.

### Logs client:
`iter_N.csv` é o log da conexão de N clientes com um servidor iterativo. Enquanto 
`par_N.csv` é a conexão de N clientes com um servidor paralelo.

Cada arquivo `csv` terá a seguinte estrutura:
| Tam arquivo | Perda de pacotes  | Tempo de download |  Sha256sum |
| -: | :-: | :-: | :-: |
|    1MB     |        c1_x0         |        c1_y0        |     1       |
|    10MB    |        c1_x1         |        c1_y1        |     1       |
|    100MB   |        c1_x2         |        c1_y2        |     1       |
|    1GB     |        c1_x3         |        c1_y3        |     1       |

Onde cada linha é uma transmissão de arquivo para um cliente `cn`

Exemplo de arquivo `csv` gerado de um teste com 2 clientes:
| Tam arquivo | Perda de pacotes | Tempo de download |  Sha256sum |
| -: | :-: | :-: | :-: |
|    1MB     |        c1_x0         |        c1_y0        |     1       |
|    1MB     |        c2_a0         |        c2_b0        |     1       |
|    10MB    |        c1_x1         |        c1_y1        |     1       |
|    10MB    |        c2_a1         |        c2_b1        |     1       |
|    100MB   |        c1_x2         |        c1_y2        |     1       |
|    100MB   |        c2_a2         |        c2_b2        |     1       |
|    1GB     |        c1_x3         |        c1_y3        |     1       |
|    1GB     |        c2_a3         |        c2_b3        |     1       |
