import csv
import json
from itertools import groupby


def group(lst, pk, fk):
    keyfunc = lambda x: x[pk]
    data = sorted(lst, key=keyfunc)
    result = {}

    for k, g in groupby(data, keyfunc):
        result[k] = [d[fk] for d in g]

    return result


def csv_to_dict(lst, key):
    result = {}

    for row in lst:
        id_ = row[key]
        del row[key]
        result[id_] = row

    return result


def load_csv(file):
    with open(file, encoding='utf-8') as f:
        return list(csv.DictReader(f))


def join(col, lst, pk, key, default=None):
    if default is None:
        default = []

    for obj in col:
        obj[key] = lst.get(obj[pk], default)


def ajusta_disciplinas(disciplinas):
    for d in disciplinas:
        nome, _ = d['nome'].split(' :: ')
        d['nome'] = nome
        d['capacidade'] = int(d['capacidade'])
        d['carga'] = int(d['carga']) // 18
        d['ofertada'] = bool(d['ofertada'])


def ajusta_periodo_turma(lst):
    for d in lst:
        d['periodo'] += f"-{d['turma']}:{d['curso']}"
        d['turma'] += f"-{d['curso']}"


def blacklist(disciplinas):
    lst = ['1496', '1500', '1534', ]


def main():
    alunos_file = 'alunos.csv'
    competencias_file = 'competencias.csv'
    cursadas_file = 'cursadas.csv'
    disciplinas_file = 'disciplinas.csv'
    equivalencias_file = 'equivalencias.csv'
    professores_file = 'professores.csv'
    restantes_file = 'restantes.csv'
    output_file = 'input.new.json'

    alunos = load_csv(alunos_file)
    disciplinas = load_csv(disciplinas_file)
    professores = load_csv(professores_file)
    competencias_csv = load_csv(competencias_file)
    cursadas_csv = load_csv(cursadas_file)
    equivalencias_csv = load_csv(equivalencias_file)
    restantes_csv = load_csv(restantes_file)

    competencias = group(competencias_csv, 'id_prof', 'id_disc')
    cursadas = group(cursadas_csv, 'id_aluno', 'id_disc')
    restantes = group(restantes_csv, 'id_aluno', 'id_disc')
    equivalencias = group(equivalencias_csv, 'id_disc', 'id_equiv')

    join(alunos, cursadas, 'id', 'cursadas')
    join(alunos, restantes, 'id', 'restantes')
    join(professores, competencias, 'id', 'competencias')
    join(disciplinas, equivalencias, 'id', 'equivalencias')

    ajusta_periodo_turma(disciplinas)
    ajusta_periodo_turma(alunos)

    ajusta_disciplinas(disciplinas)

    blacklist(disciplinas)

    result = {
        'disciplinas': disciplinas,
        'professores': professores,
        'alunoperfis': alunos
    }

    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(result, f)

if __name__ == '__main__':
    main()
