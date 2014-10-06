#include "Grade.h"

Grade::Grade(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) : Representacao(pBlocosTamanho, pCamadasTamanho) {
  perfisTamanho = pPerfisTamanho;

  init();
}

void Grade::init() {
  perfis.resize(perfisTamanho);
}

Grade::~Grade() {
}

double Grade::getObjectiveFunction() {
  ProfessorDisciplina *pd;

  double fo = 0;

  int triDimensional[3];
  int perfil, diaSemana;

  for (int i = 0; i < size; i++) {
    pd = at(i);

    get3DMatrix(i, triDimensional);

    diaSemana = triDimensional[1];
    perfil = triDimensional[2];

    fo += (pd->disciplina->cargaHoraria * perfis[perfil]->peso) + (100 * pd->professor->diasDisponiveis[diaSemana]);
  }

  return fo;
}