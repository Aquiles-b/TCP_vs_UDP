import os
import requests
import sys
import subprocess
import threading
import time

http_ip = None
http_port = None
file_server_port = 6666

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
	global http_ip, file_server_port
	start_time = time.time()
	process = subprocess.Popen(["sh", "-c", f"./client {http_ip} {file_server_port} tcp files/{filesize}.bin downloads/{clientid}/{filesize}.bin"])
	if (process.wait() != 0):
		print("Error on client process!\n")
		return
	end_time = time.time()
	result["time"] = end_time - start_time

def run_test_tcp_iter(clientnum: int, interval: float, filesize: str):
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
	
	print(results)
	print(total_time := end_time - start_time)

if __name__ == "__main__":
	if len(sys.argv) != 4:
		print("Correct usage: python run_test.py <http_ip> <http_port> <is_local_net>")
		exit(1)
	http_ip = sys.argv[1]
	http_port = sys.argv[2]
	local_net = bool(sys.argv[3])
	buffersizes = [1024, 4096, 16384, 16384*3]
	clientnum = [1, 2, 4, 8]
	udpwindow = [1, 4, 16, 64, 256]
	timeintervals = [0.25]
	filesizes = ["1MB", "10MB", "100MB"]
	for i in range(max(clientnum)):
		try:
			os.mkdir(f"downloads/{i}")
		except FileExistsError as e:
			print(f"Directory downloads/{i} already exists.")
	print(f"Trafego estimado: {len(buffersizes)*sum(clientnum)*(2+len(udpwindow))*len(timeintervals)/10}GB")
	for bsize in buffersizes:
		init_server(file_server_port, "tcp", bsize, "iter", local_net)
		for cnum in clientnum:
			for tinter in timeintervals:
				for fsize in filesizes:
					run_test_tcp_iter(cnum, tinter, fsize)


