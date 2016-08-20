#include "Representacao.h"

#include <algorithm>
#include <cmath>

#include "Algorithms.h"
#include "Semana.h"

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
, matriz(outro.matriz) {
}

Representacao& Representacao::operator=(const Representacao& outro) {
    blocosTamanho = outro.blocosTamanho;
    camadasTamanho = outro.camadasTamanho;
    size = outro.size;
    blocos = outro.blocos;
    camadas = outro.camadas;
    matriz = outro.matriz;

    return *this;
}

Representacao::~Representacao() {
}

void Representacao::initMatriz() {
    size = (camadasTamanho * blocosTamanho * dias_semana_util);

    matriz.resize(size, nullptr);
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

    return true;
}

void Representacao::get3DMatrix(int pLinear, int triDimensional[3]) {
    Util::get3DMatrix(pLinear, triDimensional, dias_semana_util, blocosTamanho, camadasTamanho);
}

std::tuple<int, int, int> Representacao::getCoords(int pLinear) const
{
	int coord[3];
	Util::get3DMatrix(pLinear, coord, dias_semana_util, blocosTamanho, camadasTamanho);

	return std::make_tuple(coord[0], coord[1], coord[2]);
}

std::vector<ProfessorDisciplina*>::iterator Representacao::getFirstDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, Disciplina* pDisciplina) {
    std::vector<ProfessorDisciplina*>::iterator xIter = find_if(iter, iterEnd, HorarioFindDisciplina(pDisciplina));

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

    for (int d = 0; d < dias_semana_util; d++) {
        for (int b = 0; b < blocosTamanho; b++) {
            position = getPosition(d, b, camada);
            if (matriz[position] == nullptr) {
                empties.push_back(position);
            }
        }
    }

    return empties;
}

void Representacao::clearDisciplina(ProfessorDisciplina *pProfessorDisciplina, int camada) {
    int position = -1;

    for (int d = 0; d < dias_semana_util; d++) {
        for (int b = 0; b < blocosTamanho; b++) {
            position = getPosition(d, b, camada);
            if (matriz[position] == pProfessorDisciplina) {
                matriz[position] = nullptr;
            }
        }
    }
}

void Representacao::clearSlot(int pDia, int pBloco, int pCamada)
{
	int position = getPosition(pDia, pBloco, pCamada);

	matriz[position] = nullptr;
}

int Representacao::getPositionDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, std::vector<ProfessorDisciplina*>::iterator iterFound) {
    int x = -1;

    if (iterFound != iterEnd) {
        x = iterFound - iter;
    }

    return x;
}
