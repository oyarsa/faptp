#! /usr/bin/env python3

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
            del e['periodo']
            e['dia'] = int(e['dia'])
            e['horario'] = int(e['horario'])

        jdict = {
            'nome': key,
            'eventos': eventos[i]
        }
        pdict.append(jdict)

    return pdict


def main():
    if len(sys.argv) != 3:
        print('Número inválido de argumentos')
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile) as f:
        r = csv.DictReader(f)
        events = list(r)

    periodos, eventos = group_eventos(events)
    pdict = convert_dict(periodos, eventos)

    with open(outfile, 'w') as f:
        json.dump({'periodos': pdict}, f)


if __name__ == '__main__':
    main()
