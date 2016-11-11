#! /usr/bin/env python3

import json
import sys

num_dias = 6
num_horarios = 4
horas_max = num_dias * num_horarios
pesos_constraints = [1] * 9


def print_set(aset):
    return '{' + ', '.join('"{}"'.format(x) for x in aset) + '}'


def periodos_nome(disciplinas):
    return set(str(d['periodo']) for d in disciplinas)


def disciplinas_nome(disciplinas):
    return [str(d['id']) for d in disciplinas]


def professores_nome(professores):
    return [str(p['id']) for p in professores]


def oferecidas(disciplinas):
    return [int(d['ofertada']) for d in disciplinas]


def carga_horaria(disciplinas):
    return [d['carga'] for d in disciplinas]


def contrato_prof(professores):
    key = 'creditoMaximo'
    return [p[key] if key in p else horas_max for p in professores]


def new_matrix(rows, cols, val=0):
    return [[val for _ in range(cols)] for _ in range(rows)]


def habilitados(professores, disciplinas):
    matriz = new_matrix(len(professores), len(disciplinas))

    for i, p in enumerate(professores):
        for j, d in enumerate(disciplinas):
            if d['id'] in p['competencias']:
                matriz[i][j] = 1

    return matriz


def turma_disciplina(disciplinas):
    periodos = periodos_nome(disciplinas)
    matriz = new_matrix(len(disciplinas), len(periodos))

    for i, d in enumerate(disciplinas):
        for j, c in enumerate(periodos):
            if str(d['periodo']) == c:
                matriz[i][j] = 1

    return matriz


def disponibilidade(professores):
    profmatrix = []

    for p in professores:
        if 'disponibilidade' not in p:
            matriz = new_matrix(num_horarios, num_dias, val=1)
        else:
            matriz = [h[:num_dias] for h in p['disponibilidade']]
        profmatrix.append(matriz)

    return profmatrix


def disciplinas_dificeis(disciplinas):
    return [0 for _ in disciplinas]


def preferencias_disciplinas(professores, disciplinas):
    return new_matrix(len(professores), len(disciplinas), val=1)


def preferencias_aulas(professores):
    return contrato_prof(professores)


def blocos_disciplina(disciplina):
    carga = disciplina['carga']
    return [2] * (carga // 2) + [1] * (carga % 2)


def blocos_disciplinas(disciplinas):
    return [blocos_disciplina(d) for d in disciplinas]


def print_array_of_sets(x):
    s = ['{' + ', '.join(str(j) for j in i) + '}' for i in x]
    return '[' + ', '.join(s) + ']'


def filtrar_nao_ofertadas(disciplinas):
    return [d for d in disciplinas if d['ofertada']]


def main():
    if len(sys.argv) != 3:
        print('Número inválido de argumentos')
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile, encoding='utf8') as f:
        indict = json.load(f)

    disciplinas = filtrar_nao_ofertadas(indict['disciplinas'])
    professores = indict['professores']

    with open(outfile, 'w') as f:
        print('num_horarios =', num_horarios, file=f, end=';\n\n')
        print('num_dias =', num_dias, file=f, end=';\n\n')
        print('pi =', pesos_constraints, file=f, end=';\n\n')
        print('C =', print_set(periodos_nome(disciplinas)), file=f, end=';\n\n')
        print('D =', print_set(disciplinas_nome(disciplinas)), file=f, end=';\n\n')
        print('P =', print_set(professores_nome(professores)), file=f, end=';\n\n')
        print('O =', oferecidas(disciplinas), file=f, end=';\n\n')
        print('K =', carga_horaria(disciplinas), file=f, end=';\n\n')
        print('N =', contrato_prof(professores), file=f, end=';\n\n')
        print('h =', habilitados(professores, disciplinas), file=f, end=';\n\n')
        print('H =', turma_disciplina(disciplinas), file=f, end=';\n\n')
        print('A =', disponibilidade(professores), file=f, end=';\n\n')
        print('G =', disciplinas_dificeis(disciplinas), file=f, end=';\n\n')
        print('F =', preferencias_disciplinas(professores, disciplinas), file=f, end=';\n\n')
        print('Q =', preferencias_aulas(professores), file=f, end=';\n\n')
        print('B =', print_array_of_sets(blocos_disciplinas(disciplinas)), file=f, end=';\n\n')


if __name__ == '__main__':
    main()
