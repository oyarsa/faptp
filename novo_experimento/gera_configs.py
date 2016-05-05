import itertools

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
print("Combinacoes:", num_comb)
num_maq = 38
num_configs = int(num_comb / num_maq)
print('Configurações por máquina: ', num_configs)

# Formato
# ID AGIter TaxaMut NIndiv %Cruz CruzOper NMut NTour GRASPIter GRASPNVzi GRASPAlfa NExec
param_cod = ["AI", "TM", "NI", "PC", "Xo", "NM", "NT", "GI", "GV", "GA"]


def make_id(config):
    return '.'.join(x + str(y) for x, y, in zip(param_cod, config))


def config_str(config):
    return make_id(config) + " " + " ".join(str(x) for x in config) + " " + str(num_exec)

configs = itertools.product(ag_iter, taxa_mutacao, num_indiv, porcent_cruz,
                            oper_cruz, num_mut, num_torneio, grasp_iter,
                            grasp_vizinh, grasp_alfa)

for i, config in enumerate(configs):
    num = i // num_configs
    if num >= 35:
        num += 1
    fname = str(num) + ".txt"
    with open(fname, "a") as f:
        print(config_str(config), file=f)
