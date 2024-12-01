from matplotlib import pyplot as plt
from glob import glob
import sys
import os


def get_TCP_iter_par_metrics(tcp_logs_path: str):
    tcp_logs = sorted(glob(tcp_logs_path + "/*.csv"))
    tcp_iter = dict()
    tcp_par = dict()

    tcp_logs = sorted(tcp_logs, key=lambda x: int(x.split("_")[2].split(".")[0]))

    for log in tcp_logs:
        file_name = log.split("/")[-1]
        num_clients = file_name.split("_")[1]
        tam_buffer = file_name.split("_")[2].split(".")[0]
        if ("iter" in file_name):
            aux_dict = tcp_iter
        else:
            aux_dict = tcp_par

        if (num_clients not in aux_dict):
            aux_dict[num_clients] = dict()
            aux_dict[num_clients]["tam_buffer"] = []
            aux_dict[num_clients]["temp_download"] = []
            aux_dict[num_clients]["sha256sum"] = []

        aux_dict[num_clients]["tam_buffer"].append(tam_buffer)
        temp_download = 0
        sha256sum = 0
        with open(log, "r") as f:
            lines = f.readlines()
            for line in lines[1:-1]:
                line = line.split(",")
                temp_download += float(line[2])
                sha256sum += int(line[3])
        aux_dict[num_clients]["temp_download"].append(float(temp_download/len(lines[1:])))
        aux_dict[num_clients]["sha256sum"].append(float(sha256sum/len(lines[1:])))

    return tcp_iter, tcp_par

def get_UDP_metrics(udp_logs_path: str):
    udp_logs = sorted(glob(udp_logs_path + "/*.csv"))
    udp = dict()

    udp_logs = sorted(udp_logs, key=lambda x: (int(x.split("_")[2]), int(x.split("_")[3].split(".")[0])))

    for log in udp_logs:
        file_name = log.split("/")[-1]
        num_clients = file_name.split("_")[1]
        tam_buffer = file_name.split("_")[2]
        win_size = file_name.split("_")[3].split(".")[0]

        if (num_clients not in udp):
            udp[num_clients] = dict()
            udp[num_clients]["tam_buffer"] = []
            udp[num_clients]["temp_download"] = []
            udp[num_clients]["sha256sum"] = []
            udp[num_clients]["perda_pacotes"] = []
            udp[num_clients]["num_tentativas"] = []
            udp[num_clients]["win_size"] = []

        udp[num_clients]["tam_buffer"].append(tam_buffer)
        udp[num_clients]["win_size"].append(win_size)
        temp_download = 0
        sha256sum = 0
        perda_pacotes = 0
        num_tentativas = 0
        with open(log, "r") as f:
            lines = f.readlines()
            for line in lines[1:-1]:
                line = line.split(",")
                temp_download += float(line[2])
                sha256sum += int(line[3])
                perda_pacotes += int(line[1])
                num_tentativas += int(line[0])
        udp[num_clients]["temp_download"].append(float(temp_download/len(lines[1:])))
        udp[num_clients]["sha256sum"].append(float(sha256sum/len(lines[1:])))
        udp[num_clients]["perda_pacotes"].append(float(perda_pacotes/len(lines[1:])))
        udp[num_clients]["num_tentativas"].append(float(num_tentativas/len(lines[1:])))
    
    return udp

def plot_TCP_UDP_metrics(log_path: str, output_dir: str):
    tcp_iter, tcp_par = get_TCP_iter_par_metrics(f"{log_path}/TCP")
    udp = get_UDP_metrics(f"{log_path}/UDP")

    # Plotando o tempo de atendimento dos clientes de acordo com o tamanho do buffer
    for server_type, data in {"TCP Iter": tcp_iter, "TCP Par": tcp_par}.items():
        fig, ax = plt.subplots()
        for num_clients, data in data.items():
            ax.plot(data["tam_buffer"], data["temp_download"], label=f"{server_type} {num_clients} clients")
        ax.set_xlabel("Buffer Size")
        ax.set_ylabel("Average Download Time")
        ax.legend()
        plt.savefig(f"{output_dir}/{server_type}_temp_download_by_buffer_size.png")

    fig, ax = plt.subplots()
    for num_clients, data in udp.items():
        temp_down_medio_por_buffer = []
        for buffer_size in list(dict.fromkeys(data["tam_buffer"])):
            temp_down_medio_por_buffer.append(sum([data["temp_download"][idx] 
                                                for idx, buffer in enumerate(data["tam_buffer"])
                                                   if buffer == buffer_size])/data["tam_buffer"].count(buffer_size)
                                             )
        ax.plot(list(dict.fromkeys(data["tam_buffer"])) , temp_down_medio_por_buffer, label=f"UDP {num_clients} clients")

    ax.set_title("Average Download Time by Buffer Size")
    ax.set_xlabel("Buffer Size")
    ax.set_ylabel("Average Download Time")
    ax.legend()
    plt.savefig(f"{output_dir}/UDP_temp_download_by_buffer_size.png")

    # Plotando o tempo de atendimento (eixo y) de um cliente de acordo com o buffer size (eixo x), usa o tamanho da janela do udp em cada linha
    fig, ax = plt.subplots()
    buffer_sizes = list(dict.fromkeys(udp["1"]["tam_buffer"]))
    time_for_each_win_size = []
    num_win_sizes = len(set(udp["1"]["win_size"]))
    
    for offset in range(num_win_sizes):
        time_for_each_win_size.append([])
        for idx in range(0, len(udp["1"]["temp_download"]), num_win_sizes):
            time_for_each_win_size[-1].append(udp["1"]["temp_download"][idx + offset])

    for idx, win_size in enumerate(list(dict.fromkeys(udp["1"]["win_size"]))):
        ax.plot(buffer_sizes, time_for_each_win_size[idx], label=f"Win Size {win_size}")

    ax.set_title("Average Download Time by Buffer Size and Window Size")
    ax.set_xlabel("Buffer Size")
    ax.set_ylabel("Average Download Time")
    ax.legend()
    plt.savefig(f"{output_dir}/UDP_temp_download_buffer_win_size.png")

    # Um grafico com a perda de pacotes média do udp de acordo com o numero de clientes
    fig, ax = plt.subplots()
    perda_pac_medio_clientes = []
    for num_clients, data in udp.items():
        perda_pac_medio_clientes.append(sum(data["perda_pacotes"])/len(data["perda_pacotes"]))

    ax.plot(list(udp.keys()), perda_pac_medio_clientes)
    ax.set_title("Average Packet Loss by Number of Clients")
    ax.set_xlabel("Number of Clients")
    ax.set_ylabel("Average Packet Loss")
    plt.savefig(f"{output_dir}/UDP_avg_packet_loss_by_num_clients.png")

    # Um gráfico com o tempo médio para atender um cliente com tcp iter, tcp par e udp de acordo com o numero de clientes
    fig, ax = plt.subplots()
    for server_type, data in {"TCP Iter": tcp_iter, "TCP Par": tcp_par, "UDP": udp}.items():
        ax.plot(list(data.keys()), [sum(data[num_clients]["temp_download"]) for num_clients in data.keys()], label=server_type)
    ax.set_title("Average Download Time by Number of Clients")
    ax.set_xlabel("Number of Clients")
    ax.set_ylabel("Average Download Time")
    ax.legend()
    plt.savefig(f"{output_dir}/avg_download_time_by_num_clients.png")

    # A média do sha256sum de acordo com o numero de clientes no tcp iter, tcp par (da ate pra agrupar o tcp nesse eu acho) e no udp
    fig, ax = plt.subplots()
    for server_type, data in {"TCP Iter": tcp_iter, "TCP Par": tcp_par}.items():
        ax.plot(list(data.keys()), [sum(data[num_clients]["sha256sum"]) for num_clients in data.keys()], label=server_type)

    ax.set_title("Average sha256sum by Number of Clients")
    ax.set_xlabel("Number of Clients")
    ax.set_ylabel("Average sha256sum")
    ax.legend()
    plt.savefig(f"{output_dir}/avg_sha256sum_by_num_clients_tcp.png")

    fig, ax = plt.subplots()
    ax.plot(list(udp.keys()), [sum(udp[num_clients]["sha256sum"]) for num_clients in udp.keys()], label="UDP")
    ax.set_title("Average sha256sum by Number of Clients")
    ax.set_xlabel("Number of Clients")
    ax.set_ylabel("Average sha256sum")
    ax.legend()
    plt.savefig(f"{output_dir}/avg_sha256sum_by_num_clients_udp.png")

if __name__ == '__main__':
    if (len(sys.argv) < 2):
        print(f"Usage: {sys.argv[0]} <log_path>")
        sys.exit(1)
    log_path = sys.argv[1]
    output_dir = "results"
    os.makedirs(output_dir, exist_ok=True)
    plot_TCP_UDP_metrics(log_path, output_dir)
