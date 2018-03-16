#! /usr/bin/python3

import csv
import sys
import json
from itertools import groupby


def group_eventos(events):
    eventos = []
    periodos = []
    keyfunc = lambda x: x['periodo']
    data = sorted(events, key=keyfunc)
    for k, g in groupby(data, keyfunc):
        eventos.append(list(g))
        periodos.append(k)

    return periodos, eventos


def convert_dict(periodos, eventos):
    pdict = []
    for i, key in enumerate(periodos):
        for e in eventos[i]:
            e['dia'] = int(e['dia'])
            e['horario'] = int(e['horario'])

        jdict = {
            'nome': key,
            'eventos': eventos[i]
        }
        pdict.append(jdict)

    return pdict


def convert_format(events):
    novo = []
    for e in events:
        disc, periodo = e['Disciplina'].split(' :: ')
        del e['Disciplina']
        e['disciplina'] = disc
        e['periodo'] = periodo + ' ' +  e['Turma']
        del e['Turma']

        e['professor'] = e['Professor']
        del e['Professor']

        e['dia'] = int(e['Dia na Semana']) - 1
        del e['Dia na Semana']

        horario = e['Horario']
        if horario == '19:00 - 19:50':
            e['horario'] = 0
        elif horario == '19:50 - 20:40':
            e['horario'] = 1
        elif horario == '21:00 - 21:50':
            e['horario'] = 2
        elif horario == '21:50 - 22:40':
            e['horario'] = 3
        else:
            continue

        del e['Horario']
        novo.append(e)
    return novo


def main():
    if len(sys.argv) != 3:
        print('Número inválido de argumentos')
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile, encoding='utf-8') as f:
        events = list(csv.DictReader(f))

    events = convert_format(events)

    periodos, eventos = group_eventos(events)
    pdict = convert_dict(periodos, eventos)

    with open(outfile, 'w') as f:
        json.dump({'periodos': pdict}, f)


if __name__ == '__main__':
    main()
