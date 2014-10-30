#include "Grade.h"

Grade::Grade(int pBlocosTamanho, int pCamadasTamanho, AlunoPerfil* pAlunoPerfil, Horario *pHorario) : Representacao(pBlocosTamanho, pCamadasTamanho) {
  alunoPerfil = pAlunoPerfil;
  horario = pHorario;

  init();
}

void Grade::init() {
  gradeMontada = false;
}

Grade::~Grade() {
}

bool Grade::havePreRequisitos(Disciplina *pDisciplina) {
  bool viavel = true;

  std::vector<std::string> pDisciplinaPreRequisitos = pDisciplina->preRequisitos;

  if (pDisciplinaPreRequisitos.size() > 0) {
    std::vector<std::string> disciplinasCursadas = alunoPerfil->cursadas;

    std::vector<std::string>::iterator dprIter = pDisciplinaPreRequisitos.begin();
    std::vector<std::string>::iterator dprIterEnd = pDisciplinaPreRequisitos.end();
    std::sort(dprIter, dprIterEnd);

    if (disciplinasCursadas.size() > 0) {

      std::vector<std::string>::iterator dcIter = disciplinasCursadas.begin();
      std::vector<std::string>::iterator dcIterEnd = disciplinasCursadas.end();
      std::sort(dcIter, dcIterEnd);

      std::vector<std::string>::iterator foundPreRequisitos = std::search(dcIter, dcIterEnd, dprIter, dprIterEnd);
      viavel = (foundPreRequisitos != dcIterEnd);
      //std::set_symmetric_difference(dprIter, dcIter, dprIterEnd, dcIterEnd, std::back_inserter(preRequisitosRestantes)); //std::set_symmetric_difference();

      if (!viavel) {
        std::cout << "Nao tem os pre requisitos" << std::endl;
      }
    } else {
      viavel = false;
    }
  }

  return viavel;
}

bool Grade::checkCollision(Disciplina* pDisciplina, int camada) {
  bool colisao = false;
  int currentPosition;

  ProfessorDisciplina *currentProfessorDisciplina;

  for (int i = 0; i < SEMANA; i++) {
    for (int j = 0; j < blocosTamanho; j++) {
      currentPosition = getPosition(j, i, camada);
      currentProfessorDisciplina = horario->matriz[currentPosition];

      if (currentProfessorDisciplina != NULL && currentProfessorDisciplina->disciplina == pDisciplina) {
        if (matriz[currentPosition] != NULL) {
          colisao = true;
        }
      }

    }
  }

  return (!colisao);
}

bool Grade::isViable(Disciplina* pDisciplina, int camada) {
  bool viavel = true;

  viavel = havePreRequisitos(pDisciplina);
  if (viavel) {
    viavel = checkCollision(pDisciplina, camada);
  }

  return viavel;
}

void Grade::add(Disciplina* pDisciplina, int camada) {
  int currentPosition;

  ProfessorDisciplina *currentProfessorDisciplina;

  for (int i = 0; i < SEMANA; i++) {
    for (int j = 0; j < blocosTamanho; j++) {
      currentPosition = getPosition(j, i, camada);
      currentProfessorDisciplina = horario->matriz[currentPosition];

      if (currentProfessorDisciplina != NULL && currentProfessorDisciplina->disciplina == pDisciplina) {
        matriz[currentPosition] = currentProfessorDisciplina;
      }

    }
  }
}

bool Grade::insert(Disciplina* pDisciplina) {
  return insert(pDisciplina, false);
}

bool Grade::insert(Disciplina* pDisciplina, bool force) {
  int camada, triDimensional[3];
  int x = getFirstDisciplina(pDisciplina, horario->matriz);

  bool viavel = true;

  get3DMatrix(x, triDimensional);
  camada = triDimensional[2];

  gradeMontada = false;

  viavel = isViable(pDisciplina, camada);
  if (viavel) {
    add(pDisciplina, camada);
  }

  return viavel;
}

int Grade::remove(Disciplina* pDisciplina) {
  int i = 0;
  int x = getFirstDisciplina(pDisciplina, matriz);

  while (x >= 0) {
    gradeMontada = false;

    matriz[x] = NULL;
    x = getFirstDisciplina(pDisciplina, matriz);

    i++;
  }

  return i;
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