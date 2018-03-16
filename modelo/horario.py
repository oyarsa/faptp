#! /usr/bin/env python3

from pprint import pprint
import sys
import json
import random
from jinja2 import Environment, FileSystemLoader


DIAS = ['Segunda', 'Terça', 'Quarta', 'Quinta', 'Sexta', 'Sábado']
NUM_DIAS = 6
NUM_HORARIOS = 4
CSS_FILE = 'estilo.css'
NOME_CORES = ['vermelho', 'amarelo', 'azul', 'verde', 'roxo', 'rosa', 'cinza',
              'azul2', 'cinza2']
PESOS_FO = [2, 1.5, 3.5, 4.667, 4, 2.5, 2.333, 3.167, 1.667]

def conta_janelas_dia(dia, camada):
    janelas = 0
    contando = False
    contador = 0
    _, periodo = camada

    for horario in range(NUM_HORARIOS):
        if periodo[horario][dia] is not None:
            if not contando:
                contando = True
            if contando and contador > 0:
                janelas += contador
                contador = 0
        elif contando:
            contador += 1

    return janelas


def conta_janelas_camada(camada):
    janelas = 0
    for d in range(NUM_DIAS):
        janelas += conta_janelas_dia(d, camada)
    return janelas


def conta_janelas(matriz):
    # pprint.pprint(matriz)
    return sum(conta_janelas_camada(c) for c in matriz)


def is_prof_dia(matriz, professor, dia):
    # pprint.pprint(matriz)
    for _, camada in matriz:
        for horario in range(NUM_HORARIOS):
            event = camada[horario][dia]
            if event is not None and event['prof'] == professor:
                return True

    return False


def intervalos_trabalho_prof(matriz, professor):
    intervalos = 0
    contando = False
    contador = 0

    for dia in range(NUM_DIAS):
        if is_prof_dia(matriz, professor, dia):
            if not contando:
                contando = True
            if contando and contador > 0:
                intervalos += contador
                contador = 0
        elif contando:
            contador += 1

    return intervalos


def intervalos_trabalho(matriz, professores):
    return sum(intervalos_trabalho_prof(matriz, p) for p in professores)


def is_aula_dia(dia, camada):
    for horario in range(NUM_HORARIOS):
        if camada[horario][dia] is not None:
            return True

    return False


def num_dias_aula(matriz):
    return sum(sum(is_aula_dia(d, c) for d in range(NUM_DIAS))
               for _, c in matriz)


def aulas_sabado_camada(camada):
    # pprint(camada)
    sabado = NUM_DIAS - 1
    num = 0
    for horario in range(NUM_HORARIOS):
        if camada[horario][sabado] is not None:
            num += 1
    return num


def aulas_sabado(matriz):
    return sum(aulas_sabado_camada(c) for _, c in matriz)


def aulas_seguidas_disc_dia(matriz, par, dia):
    num = 0
    camada, disciplina = par

    for horario in range(NUM_HORARIOS):
        event = matriz[camada][1][horario][dia]
        if event is not None and event['disc'] == disciplina:
            num += 1

    return max(num - 2, 0)


def aulas_seguidas(matriz, disciplinas):
    return sum(sum(aulas_seguidas_disc_dia(matriz, par, dia)
                   for dia in range(NUM_DIAS))
               for par in disciplinas)


def aulas_seguidas_dificil(matriz):
    return 0


def aula_dificil_ultimo(matriz):
    return 0


def preferencias_professores(matriz, professores):
    return 0


def aulas_professores(matriz, professores):
    return 0


def calcular_fo(matriz, horarios):
    professores, disciplinas = get_professores_disciplinas(horarios)
    penalidades = [
        ('Janelas', conta_janelas(matriz)),
        ('Intervalos de trabalho', intervalos_trabalho(matriz, professores)),
        ('Dias de aula', num_dias_aula(matriz)),
        ('Sábados', aulas_sabado(matriz)),
        ('Aulas seguidas', aulas_seguidas(matriz, disciplinas)),
        ('Aulas difíceis seguidas', aulas_seguidas_dificil(matriz)),
        ('Aulas difícies no último horário', aula_dificil_ultimo(matriz)),
        ('Preferências dos professores',
         preferencias_professores(matriz, professores)),
        ('Número de aulas dos professores',
         aulas_professores(matriz, professores))
    ]

    funcao_objetivo = sum(peso * penalidade[1]
                          for peso, penalidade in zip(PESOS_FO, penalidades))
    return funcao_objetivo, penalidades


def matriz_vazia(rows, cols):
    return [[None for _ in range(cols)] for _ in range(rows)]


def disciplinas_periodo(periodo):
    disciplinas = set()
    for e in periodo['eventos']:
        disciplinas.add(e['disciplina'])
    return disciplinas


def disciplinas_cores(disciplinas):
    cores = random.sample(NOME_CORES, len(disciplinas))
    disc_cores = {}

    for i, d in enumerate(disciplinas):
        disc_cores[d] = cores[i]

    return disc_cores


def cores_periodo(periodo):
    return disciplinas_cores(disciplinas_periodo(periodo))


def get_professores_disciplinas(horarios):
    professores = []
    disciplinas = []

    for i, periodo in enumerate(horarios['periodos']):
        for event in periodo['eventos']:
            professores.append(event['professor'])
            disciplinas.append((i, event['disciplina']))

    return set(professores), set(disciplinas)


def carregar_periodos(horarios):
    periodos = []

    for periodo in horarios['periodos']:
        nome = periodo['nome']
        matriz = matriz_vazia(NUM_HORARIOS, NUM_DIAS)
        cores = cores_periodo(periodo)

        for e in periodo['eventos']:
            dia = int(e['dia'])
            horario = int(e['horario'])
            matriz[horario][dia] = {
                'prof': e['professor'],
                'disc': e['disciplina'],
                'cor': cores[e['disciplina']]
            }

        periodos.append((nome, matriz))

    def sort_func(tup):
        nome, _ = tup
        try:
            turma, curso = nome.split('-')
            return curso, turma
        except ValueError:
            return nome

    return sorted(periodos, key=sort_func)


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
    with open(sys.path[0] + '/' + CSS_FILE) as f:
        css = f.read()

    with open(outfile, 'w', encoding='utf8') as f:
        print(template.render(periodos=periodos, dias=DIAS, css=css), file=f)

    pprint(calcular_fo(periodos, horarios))

if __name__ == '__main__':
    main()
