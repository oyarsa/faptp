import itertools
import math
import os

# Configuração do script
diretorio_saida = 'config_hysst'
num_maq = 40
num_exec = 2

# Conjunto de parâmetros
max_level = [5, 15, 30]
t_start   = [1, 5, 10]
t_step    = [5, 10, 15]
it_hc     = [5, 10, 15]
it_neigh  = [3, 4, 5]

num_comb = (len(max_level) * len(t_start) * len(t_step) * len(it_hc)
            * len(it_neigh))
combinacoes = itertools.product(max_level, t_start, t_step, it_hc, it_neigh)

print("Combinacoes:", num_comb)

# Formato
# ID MaxLevel TStart TStep IterHc IterMut NExec
param_cod = ["ML", "T0", "TS", "IH", "IM"]


def grouper(iterable, n, fillvalue=None):
    "Collect data into fixed-length chunks or blocks"
    # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx"
    args = [iter(iterable)] * n
    return itertools.zip_longest(*args, fillvalue=fillvalue)


def make_id(config):
    return 'Hy-' + '.'.join(x + str(y) for x, y, in zip(param_cod, config))


def config_str(config):
    return (make_id(config) + " " + " ".join(str(x) for x in config)
            + " " + str(num_exec))


configs_todas = [config_str(c) for c in combinacoes]

completos = files = [f[:-4] for f in os.listdir('.') if f.endswith('.txt')]
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
