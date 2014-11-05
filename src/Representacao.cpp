#include "Representacao.h"

Representacao::Representacao(int pBlocosTamanho, int pCamadasTamanho) {
  blocosTamanho = pBlocosTamanho;
  camadasTamanho = pCamadasTamanho;

  initMatriz();
}

Representacao::~Representacao() {
}

void Representacao::initMatriz() {
  size = (blocosTamanho * SEMANA * camadasTamanho);

  matriz.resize(size);
  alocados.resize(size);

  for (int i = 0; i < size; i++) {
    matriz[i] = NULL;
  }
}

int Representacao::getPosition(int pBloco, int pDia, int pCamada) {
  return (pDia + (pBloco * blocosTamanho) + (pCamada * blocosTamanho * camadasTamanho));
}

ProfessorDisciplina* Representacao::at(int pBloco, int pDia, int pCamada) {
  return at(getPosition(pBloco, pDia, pCamada));
}

ProfessorDisciplina* Representacao::at(int position) {
  return matriz[position];
}

bool Representacao::insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina) {
  return insert(pBloco, pDia, pCamada, pProfessorDisciplina, false);
}

bool Representacao::insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force) {
  int position = getPosition(pBloco, pDia, pCamada);

  matriz[position] = pProfessorDisciplina;
  alocados[position] = pProfessorDisciplina->professor->id;

  return true;
}

void Representacao::get3DMatrix(int pLinear, int* triDimensional) {
  Util util;

  util.get3DMatrix(pLinear, triDimensional, blocosTamanho, camadasTamanho);
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

int Representacao::getPositionDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, std::vector<ProfessorDisciplina*>::iterator iterFound) {
  int x = -1;

  if (iterFound != iterEnd) {
    x = iterFound - iter;
  }

  return x;
}