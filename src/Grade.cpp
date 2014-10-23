#include "Grade.h"

Grade::Grade(int pBlocosTamanho, int pCamadasTamanho, AlunoPerfil* pAlunoPerfil) : Representacao(pBlocosTamanho, pCamadasTamanho) {
  alunoPerfil = pAlunoPerfil;

  init();
}

void Grade::init() {
}

Grade::~Grade() {
}

bool Grade::insert(ProfessorDisciplina* pProfessorDisciplina, Horario* pHorario) {
  return insert(pProfessorDisciplina, pHorario, false);
}

bool Grade::insert(ProfessorDisciplina* pProfessorDisciplina, Horario* pHorario, bool force) {
  int x = getFirstProfessorDisciplina(pProfessorDisciplina, pHorario->matriz);
  int triDimensional[3];
  int bloco, dia, camada;

  int currentPosition;
  ProfessorDisciplina *currentProfessorDisciplina;

  get3DMatrix(x, triDimensional);
  bloco = triDimensional[0];
  dia = triDimensional[1];
  camada = triDimensional[2];

  for (int i = 0; i < SEMANA; i++) {
    for (int j = 0; j < blocosTamanho; j++) {
      currentPosition = getPosition(j, i, camada);
      currentProfessorDisciplina = pHorario->matriz[currentPosition];

      if (currentProfessorDisciplina != NULL && currentProfessorDisciplina == pProfessorDisciplina) {
        if (matriz[currentPosition] != NULL) {
          return false;
        }
      }

    }
  }

  for (int i = 0; i < SEMANA; i++) {
    for (int j = 0; j < blocosTamanho; j++) {
      currentPosition = getPosition(j, i, camada);
      currentProfessorDisciplina = pHorario->matriz[currentPosition];

      if (currentProfessorDisciplina != NULL && currentProfessorDisciplina == pProfessorDisciplina) {
        matriz[currentPosition] = currentProfessorDisciplina;
      }

    }
  }

  return true;
}

double Grade::getObjectiveFunction() {
  ProfessorDisciplina *professorDisciplina;

  double fo = 0;

  int triDimensional[3];
  int perfil, diaSemana;

  for (int i = 0; i < size; i++) {
    professorDisciplina = at(i);

    get3DMatrix(i, triDimensional);

    diaSemana = triDimensional[1];
    perfil = triDimensional[2];

    if (professorDisciplina != NULL) {
      fo += (professorDisciplina->disciplina->cargaHoraria) + (100 * professorDisciplina->professor->diasDisponiveis[diaSemana]);
    }
  }

  return fo;
}