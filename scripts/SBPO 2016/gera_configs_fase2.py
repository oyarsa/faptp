import itertools
from math import ceil

ag_iter      = [20, 40]
# 1: PMX
# 2: OX
# 3: CX
ag_crossover = [1, 2, 3]
num_exec     = 3

num_comb = (len(ag_iter) * len(ag_crossover))
print("Combinacoes:", num_comb)
num_maq = 38
num_configs = ceil(num_comb / num_maq)

# Formato
# ID AGIter Crossover
param_cod = ["AI", "XOver"]


def xover_id(id):
    return {
        1: 'PMX',
        2: 'OX',
        3: 'CX',
    }.get(id)


def make_id(config):
    return param_cod[0] + str(config[0]) + '.' + param_cod[1] + xover_id(config[1]) + '.1'


def config_str(config):
    return make_id(config) + " " + " ".join(str(x) for x in config) + " " + str(num_exec)

configs = itertools.product(ag_iter, ag_crossover)

for i, config in enumerate(configs, 1):
    fname = str(i) + ".txt"
    with open(fname, "a") as f:
        print(config_str(config), file=f)
