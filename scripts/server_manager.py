from http.server import BaseHTTPRequestHandler, HTTPServer
import subprocess
import json

# Variável global para armazenar o processo ativo do servidor C++
active_server_process = None

class ServerHandler(BaseHTTPRequestHandler):
	def _send_response(self, code, message):
		"""Helper para enviar respostas HTTP."""
		self.send_response(code)
		self.send_header("Content-Type", "application/json")
		self.end_headers()
		self.wfile.write(json.dumps({"message": message}).encode())

	def do_POST(self):
		global active_server_process
		
		# Lê e processa o corpo da requisição
		content_length = int(self.headers["Content-Length"])
		post_data = self.rfile.read(content_length)
		try:
			request_data = json.loads(post_data.decode())
			buffer_size = request_data.get("buffer_size")
			protocol = request_data.get("protocol")
			port = request_data.get("port")
			protocol_arg = request_data.get("protocol_arg") # Tamanho janela UDP ou iter/par TCP
			localnet = request_data.get("localnet") # Rede local
		except (json.JSONDecodeError, KeyError):
			self._send_response(400, "Invalid request format.")
			return

		# Derruba o servidor ativo, se existir
		if active_server_process:
			active_server_process.terminate()
			active_server_process.wait()
			active_server_process = None

		# Inicia o servidor C++ com os argumentos fornecidos
		try:
			active_server_process = subprocess.Popen(
				["sh", "-c", f"./server 127.0.0.1 {str(port)} {str(protocol)} {str(buffer_size)} {str(protocol_arg)}", f"> logs/server/{protocol_arg}_{buffer_size}.txt"],
			)
			print(f"Process pid: {active_server_process.pid}")
			self._send_response(200, "Server started successfully.")
		except Exception as e:
			self._send_response(500, f"Failed to start server: {e}")

	def do_DELETE(self):
		global active_server_process

		# Derruba o servidor ativo, se existir
		if active_server_process:
			active_server_process.terminate()
			active_server_process.wait()
			active_server_process = None
			self._send_response(200, "Server stopped successfully.")
		else:
			self._send_response(404, "No active server to stop.")

def run_http_server(host="localhost", port=8080):
	"""Inicia o servidor HTTP."""
	server = HTTPServer((host, port), ServerHandler)
	print(f"HTTP server running on {host}:{port}")
	try:
		server.serve_forever()
	except KeyboardInterrupt:
		pass
	server.server_close()
	print("HTTP server stopped.")

if __name__ == "__main__":
	run_http_server()
