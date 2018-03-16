import itertools
import math
import os

# Configuração do script
diretorio_saida = 'config_wdju'
num_maq = 1
num_exec = 10

# Conjunto de parâmetros
stag_limit  = [10, 20, 30]
jump_factor = [0.001, 0.002, 0.005]

num_comb = (len(stag_limit) * len(jump_factor))
combinacoes = itertools.product(stag_limit, jump_factor)

print("Combinacoes:", num_comb)

# Formato
# ID StagLimit JumpFactor NExec
param_cod = ["SL", "JF"]


def grouper(iterable, n, fillvalue=None):
    "Collect data into fixed-length chunks or blocks"
    # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx"
    args = [iter(iterable)] * n
    return itertools.zip_longest(*args, fillvalue=fillvalue)


def make_id(config):
    return 'W-' + '.'.join(x + str(y) for x, y, in zip(param_cod, config))


def config_str(config):
    return (make_id(config) + " " + " ".join(str(x) for x in config)
            + " " + str(num_exec))


configs_todas = [config_str(c) for c in combinacoes]

completos = files = [f[:-4] for f in os.listdir('.') if f.endswith('.csv')]
configs = [c for c in configs_todas if c.split()[0] not in completos]

print("Numero configs: ", len(configs))
num_configs = math.ceil(len(configs) / num_maq)

print('Configurações por máquina: ', num_configs)
grupos = grouper(configs, num_configs)

os.makedirs(diretorio_saida, exist_ok=True)

for i, configs in enumerate(grupos, 1):
    for config in configs:
        if not config:
            continue
        fname = diretorio_saida + "\\" + str(i) + ".txt"
        with open(fname, "a") as f:
            print(config, file=f)
