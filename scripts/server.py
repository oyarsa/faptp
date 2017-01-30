import os
import datetime
from flask import Flask, request, abort

RESULT_FOLDER = 'resultados'
LOGFILE = 'log.txt'
app = Flask(__name__)


def salvar_arquivo(nome, result):
    os.makedirs(RESULT_FOLDER, exist_ok=True)
    filename = os.path.join(RESULT_FOLDER, nome) + '.csv'

    with open(filename, 'w') as f:
        print(result, file=f)

    with open(LOGFILE, 'a') as f:
        print(datetime.datetime.now(), nome, file=f)


@app.route('/')
def home():
    return 'OK'


@app.route('/submit', methods=['POST'])
def submit():
    result = request.form['result']
    nome = request.form['nome']

    if result is None or nome is None:
        abort(401)
    else:
        salvar_arquivo(nome, result)
        print(result, nome)
        return 'OK'


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
