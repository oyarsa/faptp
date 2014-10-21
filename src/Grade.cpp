#include "Grade.h"

Grade::Grade(int pBlocosTamanho, int pCamadasTamanho, AlunoPerfil* pAlunoPerfil) : Representacao(pBlocosTamanho, pCamadasTamanho) {
  alunoPerfil = pAlunoPerfil;

  init();
}

void Grade::init() {
}

Grade::~Grade() {
}

bool Grade::insert(int pBloco, int pDia, int pCamada, Horario* pHorario) {
  return insert(pBloco, pDia, pCamada, pHorario, false);
}

bool Grade::insert(int pBloco, int pDia, int pCamada, Horario* pHorario, bool force) {
  ProfessorDisciplina *professorDisciplina = at(pBloco, pDia, pCamada);
  return true;
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

    fo += (pd->disciplina->cargaHoraria) + (100 * pd->professor->diasDisponiveis[diaSemana]);
  }

  return fo;
}