

import itertools
import os

diretorio = 'config'

ag_iter      = [20, 40]
taxa_mutacao = [15, 30]
num_indiv    = [20, 40]
porcent_cruz = [20, 30]
oper_cruz    = ['PMX', 'OX', 'CX']
num_mut      = [2, 4]
num_torneio  = [2, 4]
grasp_iter   = [15, 25]
grasp_vizinh = [2, 4]
grasp_alfa   = [20, 40, 60]
num_exec     = 10

num_comb = (len(ag_iter) * len(taxa_mutacao) * len(num_indiv)
            * len(porcent_cruz) * len(oper_cruz)
            * len(num_mut) * len(num_torneio) * len(grasp_iter)
            * len(grasp_vizinh) * len(grasp_alfa))
num_maq = 39

print("Combinacoes:", num_comb)

# Formato
# ID AGIter TaxaMut NIndiv %Cruz CruzOper NMut NTour GRASPIter GRASPNVzi GRASPAlfa NExec
param_cod = ["AI", "TM", "NI", "PC", "Xo", "NM", "NT", "GI", "GV", "GA"]

def grouper(iterable, n, fillvalue=None):
    "Collect data into fixed-length chunks or blocks"
    # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx"
    args = [iter(iterable)] * n
    return itertools.zip_longest(*args, fillvalue=fillvalue)

def make_id(config):
    return '.'.join(x + str(y) for x, y, in zip(param_cod, config))


def config_str(config):
    return make_id(config) + " " + " ".join(str(x) for x in config) + " " + str(num_exec)

configs_todas = [config_str(c) for c in itertools.product(ag_iter, taxa_mutacao, num_indiv, porcent_cruz,
                            oper_cruz, num_mut, num_torneio, grasp_iter,
                            grasp_vizinh, grasp_alfa)]
completos = files = [f[:-4] for f in os.listdir('.') if f.endswith('.txt')]
configs = [c for c in configs_todas if c.split()[0] not in completos]
print("Numero configs: ", len(configs))
num_configs = int(len(configs) / num_maq)
print('Configurações por máquina: ', num_configs)
grupos = grouper(configs, num_configs)

os.makedirs(diretorio, exist_ok=True)

for i, configs in enumerate(grupos):
    for config in configs:
        if not config:
            continue
        fname = diretorio + "\\" + str(i) + ".txt"
        with open(fname, "a") as f:
            print(config, file=f)
