#include "Horario.h"

Horario::Horario(int pBlocosTamanho, int pCamadasTamanho) : Representacao(pBlocosTamanho, pCamadasTamanho) {
}

Horario::~Horario() {
}

bool Horario::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina) {
  return insert(pDia, pBloco, pCamada, pProfessorDisciplina, false);
}

bool Horario::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force) {
  int position = getPosition(pDia, pBloco, pCamada);
  bool professorAlocado = false;

std::cout << "(" << position << ")" << std::endl;
  if (alocados[position] == "" || force) {
    int positionCamada;

    for (int i = 0; i < camadasTamanho; i++) {
      positionCamada = getPosition(pDia, pBloco, i);
      if (alocados[positionCamada] == pProfessorDisciplina->id) {
        professorAlocado = true;
        break;
      }
    }

    if (!professorAlocado || force) {
      return Representacao::insert(pDia, pBloco, pCamada, pProfessorDisciplina, force);
    }
  }

  return false;
}

double Horario::getObjectiveFunction() {

}