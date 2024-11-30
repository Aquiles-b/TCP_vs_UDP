#!/bin/bash

# Define a função para criar diretórios e arquivos
criar_estrutura() {
    base_dir="logs"
    
    # Criação de diretórios
    mkdir -p "$base_dir/server/rede_externa/TCP"
    mkdir -p "$base_dir/server/rede_externa/UDP"
    mkdir -p "$base_dir/server/rede_local/TCP"
    mkdir -p "$base_dir/server/rede_local/UDP"
    mkdir -p "$base_dir/client/rede_externa/TCP"
    mkdir -p "$base_dir/client/rede_externa/UDP"
    mkdir -p "$base_dir/client/rede_local/TCP"
    mkdir -p "$base_dir/client/rede_local/UDP"
}

# Executa a função para criar a estrutura
criar_estrutura

# Exibe a estrutura criada
echo "Estrutura de diretórios e arquivos criada com sucesso:"
tree logs || ls -R logs  # Use tree se disponível, caso contrário, use ls
