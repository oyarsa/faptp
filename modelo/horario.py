#! /usr/bin/env python3

import sys
import json
import random
from jinja2 import Environment, FileSystemLoader


dias = ['Segunda', 'Terça', 'Quarta', 'Quinta', 'Sexta', 'Sábado']
num_dias = 6
num_horarios = 4
cssfile = 'estilo.css'
nome_cores = ['vermelho', 'amarelo', 'azul', 'verde', 'roxo', 'rosa', 'cinza',
              'azul2', 'cinza2']


def matriz_vazia(rows, cols):
    return [[None for _ in range(cols)] for _ in range(rows)]


def get_disciplinas_periodo(periodo):
    disciplinas = set()
    for e in periodo['eventos']:
        disciplinas.add(e['disciplina'])
    return disciplinas


def get_disciplinas_cores(disciplinas):
    cores = random.sample(nome_cores, len(disciplinas))
    disc_cores = {}

    for i, d in enumerate(disciplinas):
        disc_cores[d] = cores[i]

    return disc_cores


def get_cores_periodo(periodo):
    return get_disciplinas_cores(get_disciplinas_periodo(periodo))


def carregar_periodos(horarios):
    periodos = []

    for periodo in horarios['periodos']:
        nome = periodo['nome']
        matriz = matriz_vazia(num_horarios, num_dias)
        cores = get_cores_periodo(periodo)

        for e in periodo['eventos']:
            dia = int(e['dia'])
            horario = int(e['horario'])
            matriz[horario][dia] = {
                'prof': e['professor'],
                'disc': e['disciplina'],
                'cor': cores[e['disciplina']]
            }

        periodos.append((nome, matriz))

    return periodos


def main():
    if len(sys.argv) != 3:
        print('Número inválido de argumentos')
        sys.exit(1)

    infile = sys.argv[1]
    outfile = sys.argv[2]

    with open(infile, encoding='utf8') as f:
        horarios = json.load(f)

    periodos = carregar_periodos(horarios)

    env = Environment(loader=FileSystemLoader(sys.path[0]))
    template = env.get_template('template.html')

    with open(sys.path[0] + '/' + cssfile) as f:
        css = f.read()

    with open(outfile, 'w', encoding='utf8') as f:
        print(template.render(periodos=periodos, dias=dias, css=css), file=f)

if __name__ == '__main__':
    main()
