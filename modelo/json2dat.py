#! /usr/bin/env python3

import json
import sys
import io

num_dias = 6
num_horarios = 4
horas_max = num_dias * num_horarios
pesos_constraints = [1] * 9


def print_set(aset):
    out = io.StringIO()
    out.write('{')
    out.write(', '.join('"%s"' % (str(x)) for x in aset))
    out.write('}')
    r = out.getvalue()
    out.close()
    return r


def get_periodos(disciplinas):
    return sorted(set(str(d['periodo']) for d in disciplinas))


def get_disciplinas(disciplinas):
    return sorted(set(str(d['id']) for d in disciplinas))


def get_professores(professores):
    return sorted(set(str(p['id']) for p in professores))


def get_oferecidas(disciplinas):
    return [int(d['ofertada']) for d in disciplinas]


def get_carga_horaria(disciplinas):
    return [d['carga'] for d in disciplinas]


def get_contrato_prof(professores):
    key = 'creditoMaximo'
    return [p[key] if key in p else horas_max for p in professores]


def disc_to_index(disciplinas):
    return {d['id']: i for i, d in enumerate(disciplinas)}


def prof_to_index(professores):
    return {p['id']: i for i, p in enumerate(professores)}


def new_matrix(rows, cols, val=0):
    return [[val for _ in range(cols)] for _ in range(rows)]


def get_habilitados(professores, disciplinas):
    matriz = new_matrix(len(professores), len(disciplinas))

    for i, p in enumerate(professores):
        for j, d in enumerate(disciplinas):
            if d['id'] in p['competencias']:
                matriz[i][j] = 1

    return matriz


def get_turma_disciplina(disciplinas):
    periodos = get_periodos(disciplinas)
    matriz = new_matrix(len(disciplinas), len(periodos))

    for i, d in enumerate(disciplinas):
        for j, c in enumerate(periodos):
            if str(d['periodo']) == c:
                matriz[i][j] = 1

    return matriz


def get_disponibilidade(professores):
    profmatrix = []

    for p in professores:
        if 'disponibilidade' not in p:
            matriz = new_matrix(num_horarios, num_dias, val=1)
        else:
            matriz = [h[:num_dias] for h in p['disponibilidade']]
        profmatrix.append(matriz)

    return profmatrix


def get_disciplinas_dificeis(disciplinas):
    return [0 for _ in disciplinas]


def get_preferencias_disciplinas(professores, disciplinas):
    return new_matrix(len(professores), len(disciplinas), val=1)


def get_preferencias_aulas(professores):
    return get_contrato_prof(professores)


def main():
    if len(sys.argv) != 3:
        print('Número inválido de argumentos')
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile) as f:
        indict = json.load(f)

    disciplinas = sorted(indict['disciplinas'], key=lambda x: str(x['id']))
    professores = sorted(indict['professores'], key=lambda x: str(x['id']))

    with open(outfile, 'w') as f:
        print('num_horarios =', num_horarios, file=f, end=';\n\n')
        print('num_dias =', num_dias, file=f, end=';\n\n')
        print('pi =', pesos_constraints, file=f, end=';\n\n')
        print('C =', print_set(get_periodos(disciplinas)), file=f, end=';\n\n')
        print('D =', print_set(get_disciplinas(disciplinas)), file=f, end=';\n\n')
        print('P =', print_set(get_professores(professores)), file=f, end=';\n\n')
        print('O =', get_oferecidas(disciplinas), file=f, end=';\n\n')
        print('K =', get_carga_horaria(disciplinas), file=f, end=';\n\n')
        print('N =', get_contrato_prof(professores), file=f, end=';\n\n')
        print('h =', get_habilitados(professores, disciplinas), file=f, end=';\n\n')
        print('H =', get_turma_disciplina(disciplinas), file=f, end=';\n\n')
        print('A =', get_disponibilidade(professores), file=f, end=';\n\n')
        print('G =', get_disciplinas_dificeis(disciplinas), file=f, end=';\n\n')
        print('F =', get_preferencias_disciplinas(professores, disciplinas), file=f, end=';\n\n')
        print('Q =', get_preferencias_aulas(professores), file=f, end=';\n\n')


if __name__ == '__main__':
    main()
