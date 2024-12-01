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
        aux_dict[num_clients]["temp_download"].append(float(temp_download/len(lines[1:-1])))
        aux_dict[num_clients]["sha256sum"].append(float(sha256sum/len(lines[1:-1])))

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

        if tam_buffer not in udp[num_clients]:
            udp[num_clients][tam_buffer] = dict()
            udp[num_clients][tam_buffer]["win_size"] = []
            udp[num_clients][tam_buffer]["temp_download"] = []
            udp[num_clients][tam_buffer]["sha256sum"] = []
            udp[num_clients][tam_buffer]["perda_pacotes"] = []
            udp[num_clients][tam_buffer]["num_tentativas"] = []

        udp[num_clients][tam_buffer]["win_size"].append(win_size)

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

        num_lines = int(num_clients)
        udp[num_clients][tam_buffer]["temp_download"].append(float(temp_download/num_lines))
        udp[num_clients][tam_buffer]["sha256sum"].append(float(sha256sum/num_lines))
        udp[num_clients][tam_buffer]["perda_pacotes"].append(float(perda_pacotes/num_lines))
        udp[num_clients][tam_buffer]["num_tentativas"].append(float(num_tentativas/num_lines))
    
    return udp

def plot_TCP_UDP_metrics(log_path: str, output_dir: str):
    tcp_iter, tcp_par = get_TCP_iter_par_metrics(f"{log_path}/TCP")
    udp = get_UDP_metrics(f"{log_path}/UDP")

    # Plotando o tempo de atendimento dos clientes de acordo com o tamanho do buffer
    for server_type, data in {"TCP_Iter": tcp_iter, "TCP_Par": tcp_par}.items():
        fig, ax = plt.subplots()
        for num_clients, data in data.items():
            ax.plot(data["tam_buffer"], data["temp_download"], label=f"{server_type} {num_clients} clients")
        ax.set_title(f"Average Download Time by Buffer Size ({server_type})")
        ax.set_xlabel("Buffer Size (bytes)")
        ax.set_ylabel("Average Download Time (s)")
        ax.legend()
        plt.savefig(f"{output_dir}/{server_type}_temp_download.png")

    # Mesmo para o UDP
    num_win_sizes = len(udp["1"]["1024"]["win_size"])
    dim_buffers = list(udp["1"].keys())
    fig, ax = plt.subplots(1, num_win_sizes, figsize=(15, 5))
    for num_clients, data in udp.items():
        for win_idx in range(num_win_sizes):
            temps = [x["temp_download"][win_idx] for x in data.values()]
            ax[win_idx].plot(dim_buffers, temps, label=f"{num_clients} clients")

    for idx in range(num_win_sizes):
        ax[idx].set_title(f"Win Size {list(udp["1"]["1024"]["win_size"])[idx]}")
        ax[idx].set_xlabel("Buffer Size (bytes)")
        ax[idx].set_ylabel("Average Download Time (s)")
        ax[idx].legend()

    plt.tight_layout()
    plt.savefig(f"{output_dir}/UDP_temp_download.png")

    # Um grafico com a perda de pacotes média (eixo y) do udp de acordo com o numero de clientes (eixo x)
    # cada linha do grafico representa um tamanho de buffer
    fig, ax = plt.subplots(1, num_win_sizes, figsize=(15, 5))
    for win_idx in range(num_win_sizes):
        for dim_buf in dim_buffers:
            perdas = []
            for num_clients, data in udp.items():
                perdas.append(data[dim_buf]["perda_pacotes"][win_idx])
            ax[win_idx].plot(list(udp.keys()), perdas, label=f"Buffer Size {dim_buf}")

    for idx in range(num_win_sizes):
        ax[idx].set_title(f"Win Size {list(udp["1"]["1024"]["win_size"])[idx]}")
        ax[idx].set_xlabel("Number of Clients")
        ax[idx].set_ylabel("Average Packet Loss")
        ax[idx].legend()

    plt.tight_layout()
    plt.savefig(f"{output_dir}/UDP_packet_loss.png")

    # A média do sha256sum de acordo com o numero de clientes no tcp iter, tcp par (da ate pra agrupar o tcp nesse eu acho) e no udp
    fig, ax = plt.subplots()
    for server_type, data in {"TCP Iter": tcp_iter, "TCP Par": tcp_par}.items():
        for num_clients, data in data.items():
            mean_sha256sum = sum(data["sha256sum"]) / len(data["sha256sum"])
            ax.plot(list(data.keys()), [mean_sha256sum]*len(data), label=f"{server_type} {num_clients} clients")

    ax.set_title("Average sha256sum by Number of Clients")
    ax.set_xlabel("Number of Clients")
    ax.set_ylabel("Average sha256sum (%)")
    ax.legend()
    plt.savefig(f"{output_dir}/avg_sha256sum.png")

    # UDP
    fig, ax = plt.subplots(1, num_win_sizes, figsize=(15, 5))
    for win_idx in range(num_win_sizes):
        for dim_buf in dim_buffers:
            sha256sums = []
            for num_clients, data in udp.items():
                sha256sums.append(data[dim_buf]["sha256sum"][win_idx])
            ax[win_idx].plot(list(udp.keys()), sha256sums, label=f"Buffer Size {dim_buf}")

    for idx in range(num_win_sizes):
        ax[idx].set_title(f"Win Size {list(udp["1"]["1024"]["win_size"])[idx]}")
        ax[idx].set_xlabel("Number of Clients")
        ax[idx].set_ylabel("Average sha256sum (%)")
        ax[idx].legend()

    plt.tight_layout()
    plt.savefig(f"{output_dir}/UDP_avg_sha256sum.png")

if __name__ == '__main__':
    if (len(sys.argv) < 2):
        print(f"Usage: {sys.argv[0]} <log_path>")
        sys.exit(1)
    log_path = sys.argv[1]
    output_dir = "results"
    os.makedirs(output_dir, exist_ok=True)
    plot_TCP_UDP_metrics(log_path, output_dir)
