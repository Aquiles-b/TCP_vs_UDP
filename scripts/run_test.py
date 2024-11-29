import os
import requests
import sys
import subprocess
import threading
import time
import hashlib
import csv

http_ip = None
http_port = None
file_server_port = 6666
checksums = dict()

def calculate_sha256(file_path):
	"""
	Calcula o hash SHA-256 de um arquivo.

	Args:
		file_path (str): Caminho do arquivo para calcular o hash.

	Returns:
		str: Hash SHA-256 em formato hexadecimal.
	"""
	sha256_hash = hashlib.sha256()
	try:
		with open(file_path, "rb") as file:
			# Lê o arquivo em blocos para evitar alto consumo de memória com arquivos grandes.
			for chunk in iter(lambda: file.read(4096), b""):
				sha256_hash.update(chunk)
		return sha256_hash.hexdigest()
	except FileNotFoundError:
		return "Arquivo não encontrado."
	except Exception as e:
		return f"Erro ao calcular o hash: {e}"
	
def save_results_to_csv(results, output_file):
	"""
	Salva os resultados do teste em um arquivo CSV.

	Args:
		results (dict): Resultados no formato especificado.
		output_file (str): Caminho do arquivo CSV a ser criado.
	"""
	# Estrutura de cabeçalho
	header = ["Tam arquivo", "Perda de pacotes", "Tempo de download", "Sha256sum"]

	# Lista para armazenar as linhas
	rows = []

	# Processa cada cliente e cria as linhas
	for client_id, client_data in results.items():
		if client_id == "total_time":
			continue  # Ignorar o campo "tempo_total" temporariamente

		file_size = client_data.get("filesize", "N/A")
		packet_loss = client_data.get("packet_loss", 0)
		download_time = client_data.get("time", 0)  # Tempo em float
		checksum = 1 if client_data.get("csum", False) else 0  # Converter checksum para inteiro

		# Adiciona a linha à lista
		rows.append([file_size, packet_loss, download_time, checksum])

	# Adiciona a linha do tempo total
	total_time = results.get("total_time", 0)
	rows.append(["-", "-", total_time, "-"])

	# Salva o arquivo CSV
	with open(output_file, mode="w", newline="") as file:
		writer = csv.writer(file)
		writer.writerow(header)  # Escreve o cabeçalho
		writer.writerows(rows)  # Escreve as linhas

def init_server(server_port: int, protocol: str, buffersize: int, protocol_arg: str, localnet: bool):
	global http_ip, http_port
	payload = {
		"buffer_size": buffersize,
		"protocol": protocol,
		"port": server_port,
		"protocol_arg": protocol_arg,
		"localnet": localnet
	}
	try:
		# Faz a requisição POST para o servidor HTTP
		response = requests.post(f"http://{http_ip}:{http_port}/", json=payload)

		# Verifica o status da resposta
		if response.status_code == 200:
			return f"Server started successfully: {response.json().get('message', '')}"
		elif response.status_code == 400:
			return f"Bad Request: {response.json().get('message', 'Invalid parameters')}"
		elif response.status_code == 500:
			return f"Internal Server Error: {response.json().get('message', 'Failed to start server')}"
		else:
			return f"Unexpected response: {response.status_code} - {response.text}"
	except requests.RequestException as e:
		# Trata erros de conexão ou requisição
		return f"Failed to connect to HTTP server: {e}"

def run_tcp_client(clientid: int, filesize: str, result: dict):
	global http_ip, file_server_port, checksums
	start_time = time.time()
	process = subprocess.Popen(["sh", "-c", f"./client {http_ip} {file_server_port} tcp files/{filesize}.bin downloads/{clientid}/{filesize}.bin"])
	if (process.wait() != 0):
		print("Error on client process!\n")
		return
	end_time = time.time()
	result["time"] = end_time - start_time
	result["filesize"] = filesize
	csum = calculate_sha256(f"downloads/{clientid}/{filesize}.bin")
	result["csum"] = csum == checksums[filesize] # True se os checksums baterem e falso caso contrário

def run_udp_client(clientid: int, filesize: str, result: dict):
	global http_ip, file_server_port, checksums
	start_time = time.time()
	process = subprocess.Popen(["sh", "-c", f"./client {http_ip} {file_server_port} udp files/{filesize}.bin downloads/{clientid}/{filesize}.bin"])
	if (process.wait() != 0):
		print("Error on client process!\n")
		return
	end_time = time.time()
	result["time"] = end_time - start_time
	result["filesize"] = filesize
	csum = calculate_sha256(f"downloads/{clientid}/{filesize}.bin")
	result["csum"] = csum == checksums[filesize] # True se os checksums baterem e falso caso contrário

def run_test_tcp(clientnum: int, interval: float, filesize: str, servermode: str, localnet: bool, buffersize: int):
	threads: list[threading.Thread] = []
	results = dict()
	start_time = time.time()
	for i in range(clientnum):
		results[i] = dict()
		t = threading.Thread(target=run_tcp_client, args=(i, filesize, results[i]))
		threads.append(t)
		t.start()
		time.sleep(interval)
	
	for t in threads:
		t.join()
	end_time = time.time()
	total_time = end_time - start_time
	results["total_time"] = total_time
	netdir ='rede_local' if localnet else 'rede_externa'
	save_results_to_csv(results, f"logs/client/{netdir}/TCP/{servermode}_{clientnum}_{buffersize}.csv")

def run_test_udp(clientnum: int, interval: float, filesize: str, winsize: int, localnet: bool, buffersize: int):
	threads: list[threading.Thread] = []
	results = dict()
	start_time = time.time()
	for i in range(clientnum):
		results[i] = dict()
		t = threading.Thread(target=run_udp_client, args=(i, filesize, results[i]))
		threads.append(t)
		t.start()
		time.sleep(interval)
	
	for t in threads:
		t.join()
	end_time = time.time()
	total_time = end_time - start_time
	results["total_time"] = total_time
	netdir ='rede_local' if localnet else 'rede_externa'
	save_results_to_csv(results, f"logs/client/{netdir}/UDP/iter_{clientnum}_{buffersize}_{winsize}.csv")

if __name__ == "__main__":
	if len(sys.argv) != 4:
		print("Correct usage: python run_test.py <http_ip> <http_port> <is_local_net>")
		exit(1)
	http_ip = sys.argv[1]
	http_port = sys.argv[2]
	local_net = bool(sys.argv[3])
	buffersizes = [1024, 4096, 16384, 16384*3]
	# clientnum = [1, 2, 4, 8]
	clientnum = [1]
	udpwindow = [4, 16, 64, 256]
	timeintervals = [0]
	filesizes = ["1MB", "10MB", "100MB"]
	for fsize in filesizes:
		checksums[fsize] = calculate_sha256(f"files/{fsize}.bin")

	for i in range(max(clientnum)):
		try:
			os.mkdir(f"downloads/{i}")
		except FileExistsError as e:
			print(f"Directory downloads/{i} already exists.")
	print(f"Trafego estimado: {len(buffersizes)*sum(clientnum)*(2+len(udpwindow))*len(timeintervals)/10}GB")
	print("Iniciando testes.")
	start_time = time.time()
	print(f"Realizando testes TCP")
	for servermode in ["iter", "par"]:
		break
		for bsize in buffersizes:
			file_server_port += 1 # Utiliza portas diferentes para o servidor (evita erro de bind)
			print(f"Iniciando servidor: {http_ip}:{file_server_port} TCP com buffer de {bsize}B no modo {servermode}")
			init_server(file_server_port, "tcp", bsize, servermode, local_net)
			print("Servidor iniciado")
			for cnum in clientnum:
				for tinter in timeintervals:
					for fsize in filesizes:
						run_test_tcp(cnum, tinter, fsize, servermode, local_net, bsize)
	for winsize in udpwindow:
		for bsize in buffersizes:
			file_server_port += 1 # Utiliza portas diferentes para o servidor (evita erro de bind)
			print(f"Iniciando servidor: {http_ip}:{file_server_port} UDP com buffer de {bsize}B com janela de {winsize} segmentos")
			init_server(file_server_port, "udp", bsize, winsize, local_net)
			break
			print("Servidor iniciado")
			for cnum in clientnum:
				for tinter in timeintervals:
					for fsize in filesizes:
						run_test_udp(cnum, tinter, fsize, winsize, local_net, bsize)
		break
	end_time = time.time()


