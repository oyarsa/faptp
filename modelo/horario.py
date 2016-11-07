#! /usr/bin/env python3

import sys
import json
from jinja2 import Environment, FileSystemLoader


dias = ['Segunda', 'Terça', 'Quarta', 'Quinta', 'Sexta', 'Sábado']
num_dias = 6
num_horarios = 4
cssfile = 'estilo.css'


def matriz_vazia(rows, cols):
    return [[None for _ in range(cols)] for _ in range(rows)]


def carregar_periodos(horarios):
    periodos = []

    for periodo in horarios['periodos']:
        nome = periodo['nome']
        matriz = matriz_vazia(num_horarios, num_dias)

        for e in periodo['eventos']:
            dia = int(e['dia'])
            horario = int(e['horario'])
            matriz[horario][dia] = {
                'prof': e['professor'],
                'disc': e['disciplina']
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

    env = Environment(loader=FileSystemLoader('.'))
    template = env.get_template('template.html')

    with open(cssfile) as f:
        css = f.read()

    with open(outfile, 'w', encoding='utf8') as f:
        print(template.render(periodos=periodos, dias=dias, css=css), file=f)

if __name__ == '__main__':
    main()
