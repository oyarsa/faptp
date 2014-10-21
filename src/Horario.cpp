#include "Horario.h"

Horario::Horario(int pBlocosTamanho, int pCamadasTamanho) : Representacao(pBlocosTamanho, pCamadasTamanho) {
}

Horario::~Horario() {
}

bool Horario::insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina) {
  return Representacao::insert(pBloco, pDia, pCamada, pProfessorDisciplina);
}

bool Horario::insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force) {
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
      return Representacao::insert(pBloco, pDia, pCamada, pProfessorDisciplina, force);
    }
  }
  
  return false;
}

double Horario::getObjectiveFunction() {

}