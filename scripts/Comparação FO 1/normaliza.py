#!/usr/bin/python3

import csv
import os
import sys

if len(sys.argv) != 3:
    print('Número inválido de argumentos')
    print('Usage: ./normaliza.py <diretorio entrada> <diretorio saida>')

inputdir = sys.argv[1]
outdir = sys.argv[2]

os.makedirs(outdir, exist_ok=True)
entradas = [f for f in os.listdir(inputdir) if f.endswith('.csv')]

for filename in entradas:
    filepath = os.path.join(inputdir, filename)
    with open(filepath, encoding='utf-8') as f:
        linhas = list(csv.DictReader(f))

        for linha in linhas:
            linha[' FO'] = round(float(linha[' FO']) * 0.984796)

    newfile = os.path.join(outdir, filename)
    with open(newfile, 'w', encoding='utf-8') as f:
        print('Writing', newfile)
        fieldnames = ['ID Algoritmo', ' Numero execucao', ' Tempo total', ' FO']
        writer = csv.DictWriter(f, fieldnames=fieldnames, lineterminator='\n')

        writer.writeheader()
        writer.writerows(linhas)

