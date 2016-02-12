#include "Representacao.h"

Representacao::Representacao(int pBlocosTamanho, int pCamadasTamanho) {
    blocosTamanho = pBlocosTamanho;
    camadasTamanho = pCamadasTamanho;

    initMatriz();
}

Representacao::Representacao(const Representacao& outro)
: blocosTamanho(outro.blocosTamanho)
, camadasTamanho(outro.camadasTamanho)
, size(outro.size)
, blocos(outro.blocos)
, camadas(outro.camadas)
, matriz(outro.matriz)
, alocados(outro.alocados) {
}

Representacao& Representacao::operator=(const Representacao& outro) {
    blocosTamanho = outro.blocosTamanho;
    camadasTamanho = outro.camadasTamanho;
    size = outro.size;
    blocos = outro.blocos;
    camadas = outro.camadas;
    matriz = outro.matriz;
    alocados = outro.alocados;

    return *this;
}

Representacao::~Representacao() {
}

void Representacao::initMatriz() {
    size = (camadasTamanho * blocosTamanho * SEMANA);

    matriz.resize(size);
    alocados.resize(size);

    for (int i = 0; i < size; i++) {
        matriz[i] = NULL;
    }
}

int Representacao::getPosition(int pDia, int pBloco, int pCamada) {
    return (pBloco + (pDia * blocosTamanho) + (pCamada * blocosTamanho * SEMANA));
}

ProfessorDisciplina* Representacao::at(int pDia, int pBloco, int pCamada) {
    return at(getPosition(pDia, pBloco, pCamada));
}

ProfessorDisciplina* Representacao::at(int position) {
    return matriz[position];
}

bool Representacao::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina) {
    return insert(pDia, pBloco, pCamada, pProfessorDisciplina, false);
}

bool Representacao::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force) {
    int position = getPosition(pDia, pBloco, pCamada);

    matriz[position] = pProfessorDisciplina;
    alocados[position] = pProfessorDisciplina->professor->id;

    return true;
}

void Representacao::get3DMatrix(int pLinear, int* triDimensional) {
    Util util;

    util.get3DMatrix(pLinear, triDimensional, SEMANA, blocosTamanho, camadasTamanho);
}

std::vector<ProfessorDisciplina*>::iterator Representacao::getFirstDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, Disciplina* pDisciplina) {
    std::vector<ProfessorDisciplina*>::iterator xIter = std::find_if(iter, iterEnd, HorarioFindDisciplina(pDisciplina));

    return xIter;
}

int Representacao::getFirstDisciplina(Disciplina* pDisciplina) {
    return getFirstDisciplina(pDisciplina, matriz);
}

int Representacao::getFirstDisciplina(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> pMatriz) {
    int x;
    std::vector<ProfessorDisciplina*>::iterator mIter = pMatriz.begin();
    std::vector<ProfessorDisciplina*>::iterator mIterEnd = pMatriz.end();

    std::vector<ProfessorDisciplina*>::iterator xIter = getFirstDisciplina(mIter, mIterEnd, pDisciplina);
    x = getPositionDisciplina(mIter, mIterEnd, xIter);

    return x;
}

std::vector<int> Representacao::getAllEmpty(int camada) {
    int position = 0;
    std::vector<int> empties;

    for (int d = 0; d < SEMANA; d++) {
        for (int b = 0; b < blocosTamanho; b++) {
            position = getPosition(d, b, camada);
            if (matriz[position] == NULL) {
                empties.push_back(position);
            }
        }
    }

    return empties;
}

void Representacao::clearDisciplina(ProfessorDisciplina *pProfessorDisciplina, int camada) {
    int position = -1;

    for (int d = 0; d < SEMANA; d++) {
        for (int b = 0; b < blocosTamanho; b++) {
            position = getPosition(d, b, camada);
            if (matriz[position] == pProfessorDisciplina) {
                matriz[position] = NULL;
            }
        }
    }
}

int Representacao::getPositionDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, std::vector<ProfessorDisciplina*>::iterator iterFound) {
    int x = -1;

    if (iterFound != iterEnd) {
        x = iterFound - iter;
    }

    return x;
}