import os
import random

def create_files():
    # Definir os tamanhos dos arquivos em bytes
    sizes = {
        "1MB.bin": 1 * 1_000_000,
        "10MB.bin": 10 * 1_000_000,
        "100MB.bin": 100 * 1_000_000,
    }

    # Diretório para salvar os arquivos
    directory = "files"
    os.makedirs(directory, exist_ok=True)

    # Escolhe uma seed para gerar os arquivos
    random.seed(42)

    # Gera os arquivos
    for filename, size in sizes.items():
        filepath = os.path.join(directory, filename)
        with open(filepath, "wb") as file:
            file.write(random.randbytes(size))
        print(f"Arquivo criado: {filepath} ({size / (1_000_000):.2f} MB)")

def gen_structure():
    base_dir = "logs"
    os.makedirs(f"{base_dir}/server/rede_externa/TCP", exist_ok=True)
    os.makedirs(f"{base_dir}/server/rede_externa/UDP", exist_ok=True)
    os.makedirs(f"{base_dir}/server/rede_local/TCP", exist_ok=True)
    os.makedirs(f"{base_dir}/server/rede_local/UDP", exist_ok=True)
    os.makedirs(f"{base_dir}/client/rede_externa/TCP", exist_ok=True)
    os.makedirs(f"{base_dir}/client/rede_externa/UDP", exist_ok=True)
    os.makedirs(f"{base_dir}/client/rede_local/TCP", exist_ok=True)
    os.makedirs(f"{base_dir}/client/rede_local/UDP", exist_ok=True)

    print("Estrutura de diretórios criada com sucesso:")
    os.system("tree logs || ls -R logs")

if __name__ == "__main__":
    create_files()
    gen_structure()
