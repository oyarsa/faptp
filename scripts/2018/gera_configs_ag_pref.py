#!/usr/bin/python3
import itertools
import math
import os

# Configuração do script
diretorio_saida = 'config_ag_pref'
num_maq = 1
num_exec = 10

# Conjunto de parâmetros
taxa_mutacao = [15, 30]
num_indiv    = [40, 80]
porcent_cruz = [20, 30]
oper_cruz    = ['PMX', 'OX', 'CX']
num_mut      = [2, 4]
num_torneio  = [2, 4]
grasp_iter = [15]
grasp_vizinh = [2]
grasp_alfa   = [20]
ag_iter = [500]
tipo_fo = ['pref']

num_comb = (len(taxa_mutacao) * len(num_indiv)
            * len(porcent_cruz) * len(oper_cruz)
            * len(num_mut) * len(num_torneio) * len(grasp_iter)
            * len(grasp_vizinh) * len(grasp_alfa) * len(ag_iter) * len(tipo_fo))
combinacoes = itertools.product(
    taxa_mutacao, num_indiv, porcent_cruz,
    oper_cruz, num_mut, num_torneio, grasp_iter,
    grasp_vizinh, grasp_alfa, ag_iter, tipo_fo)

print("Combinacoes:", num_comb)

# Formato
# ID TaxaMut NIndiv %Cruz CruzOper NMut NTour GIter GNViz GAlfa AGIter TipoFO NExec
param_cod = ["TM", "NI", "PC", "Xo", "NM", "NT", "GI", "GV", "GA", "AI", "FO"]


def grouper(iterable, n, fillvalue=None):
    "Collect data into fixed-length chunks or blocks"
    # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx"
    args = [iter(iterable)] * n
    return itertools.zip_longest(*args, fillvalue=fillvalue)


def make_id(config):
    return 'AG-' + '.'.join(x + str(y) for x, y, in zip(param_cod, config))


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
        fname = diretorio_saida + "/" + str(i) + ".txt"
        with open(fname, "a") as f:
            print(config, file=f)
