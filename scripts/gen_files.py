import os

def create_files():
    # Definir os tamanhos dos arquivos em bytes
    sizes = {
        "1MB.bin": 1 * 1_000_000,
        "10MB.bin": 10 * 1_000_000,
        "100MB.bin": 100 * 1_000_000,
        "1GB.bin": 1000 * 1_000_000,
    }

    # Diretório para salvar os arquivos
    directory = "files"
    os.makedirs(directory, exist_ok=True)

    # Gera os arquivos
    for filename, size in sizes.items():
        filepath = os.path.join(directory, filename)
        with open(filepath, "wb") as file:
            file.write(os.urandom(size))  # Gera dados aleatórios para preencher o arquivo
        print(f"Arquivo criado: {filepath} ({size / (1_000_000):.2f} MB)")

if __name__ == "__main__":
    create_files()
