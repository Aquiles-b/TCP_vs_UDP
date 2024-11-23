# Comunicação
Servidor aceita cliente
Cliente (nome do arquivo) -> Servidor 
Servidor abre o arquivo e 

constexpr size_t bufferlen = 1480;

typedef mensagem uint8_t;

enum MessageType {
	ERROR,
	DATA,
	DOWNLOAD
}

enum MessageType getMessageType(uint8_t *buffer) {
	return buffer[0];
}

mensagem m = new uint_8[1024];
/*
struct mensagem {
	enum MessageType mType;
	uint8_t buffer[bufferlen];
}
*/

