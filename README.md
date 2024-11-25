# Comunicação
## TCP
- Cliente inicia conexão com servidor
- Cliente envia o nome do arquivo que deseja baixar
- Servidor abre o arquivo
- Servidor envia mensagem com tamanho do arquivo e tamanho do buffer de transmissão de arquivo
- Cliente recebe dados de transmissão, aloca buffer e se prepara para receber dados
- Servidor envia dados do arquivo para o cliente
- Servidor encerra conexão
- Cliente fecha arquivo
