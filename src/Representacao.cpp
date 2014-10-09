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

void Representacao::insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina) {
  insert(pBloco, pDia, pCamada, pProfessorDisciplina, false);
}

void Representacao::insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force) {
  int position = getPosition(pBloco, pDia, pCamada);
  bool professorAlocado = false;

  if (alocados[position] == "" || force) {
    int positionCamada;

    for (int i = 0; i < camadasTamanho; i++) {
      positionCamada = getPosition(pBloco, pDia, i);
      if (alocados[positionCamada] == pProfessorDisciplina->id) {
        professorAlocado = true;
        break;
      }
    }

    if (professorAlocado || force) {
      matriz[position] = pProfessorDisciplina;
      alocados[position] = pProfessorDisciplina->professor->id;
    }
  }
  //matriz.insert((matriz.begin() + getPosition(pBloco, pDia, pCamada)), pProfessorDisciplina);
}

void Representacao::get3DMatrix(int pLinear, int* triDimensional) {
  Util util;
  
  util.get3DMatrix(pLinear, triDimensional, blocosTamanho, camadasTamanho);
}