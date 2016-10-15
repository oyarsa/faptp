import itertools
import os

# Configuração do script
diretorio_saida = 'config'
num_maq = 40
num_exec = 10

# Conjunto de parâmetros
frac_time = [100, 200]
alfa      = [0.97, 0.935]
t0        = [0.5, 1, 2]
sa_iter   = [100, 150]
sa_reaq   = [250, 500]
sa_chance = [0, 1]
ils_iter  = [10, 30]
ils_pmax  = [10, 15, 30]
ils_p0    = [1, 3]

num_comb = (len(frac_time) * len(alfa) * len(t0) * len(sa_iter) * len(sa_reaq)
            * len(sa_chance) * len(ils_iter) * len(ils_pmax) * len(ils_p0))
combinacoes = itertools.product(frac_time, alfa, t0, sa_iter, sa_reaq,
                                sa_chance, ils_iter, ils_pmax, ils_p0)

print("Combinacoes:", num_comb)

# Formato
# ID FracTime Alfa t0 SAiter SAreaq SAchances ILSiter ILSpmax ILSp0 NExe
param_cod = ["FT", "af", "t0", "SI", "SR", "SC", "II", "PM", "P0"]


def grouper(iterable, n, fillvalue=None):
    "Collect data into fixed-length chunks or blocks"
    # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx"
    args = [iter(iterable)] * n
    return itertools.zip_longest(*args, fillvalue=fillvalue)


def make_id(config):
    return '.'.join(x + str(y) for x, y, in zip(param_cod, config))


def config_str(config):
    return (make_id(config) + " " + " ".join(str(x) for x in config)
            + " " + str(num_exec))


configs_todas = [config_str(c) for c in combinacoes]

completos = files = [f[:-4] for f in os.listdir('.') if f.endswith('.txt')]
configs = [c for c in configs_todas if c.split()[0] not in completos]

print("Numero configs: ", len(configs))
num_configs = int(len(configs) / num_maq)

print('Configurações por máquina: ', num_configs)
grupos = grouper(configs, num_configs)

os.makedirs(diretorio_saida, exist_ok=True)

for i, configs in enumerate(grupos):
    for config in configs:
        if not config:
            continue
        fname = diretorio_saida + "\\" + str(i) + ".txt"
        with open(fname, "a") as f:
            print(config, file=f)
