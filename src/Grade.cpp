#include "Grade.h"

Grade::Grade(int pBlocosTamanho, AlunoPerfil* pAlunoPerfil, Horario *pHorario) : Representacao(pBlocosTamanho, 1) {
  alunoPerfil = pAlunoPerfil;
  horario = pHorario;

  init();
}

void Grade::init() {
  fo = 0;
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

bool Grade::checkCollision(Disciplina* pDisciplina, int pCamada) {
  bool colisao = false;
  int currentPositionHorario;
  int currentPositionGrade;

  ProfessorDisciplina *currentProfessorDisciplina;

  for (int i = 0; i < SEMANA; i++) {
    for (int j = 0; j < blocosTamanho; j++) {
      currentPositionHorario = getPosition(i, j, pCamada);
      currentPositionGrade = getPosition(i, j, 0);

      currentProfessorDisciplina = horario->matriz[currentPositionHorario];

      if (currentProfessorDisciplina != NULL && currentProfessorDisciplina->disciplina == pDisciplina) {

        professorDisciplinaTemp = currentProfessorDisciplina;

        if (matriz[currentPositionGrade] != NULL) {
          colisao = true;
        }
      }

    }
  }

  return (!colisao);
}

bool Grade::isViable(Disciplina* pDisciplina, int pCamada) {
  bool viavel = true;

  viavel = havePreRequisitos(pDisciplina);
  viavel = checkCollision(pDisciplina, pCamada) && viavel;

  return viavel;
}

void Grade::add(Disciplina* pDisciplina, int pCamada) {
  int currentPositionHorario;
  int currentPositionGrade;

  ProfessorDisciplina *currentProfessorDisciplina;

  for (int i = 0; i < SEMANA; i++) {
    for (int j = 0; j < blocosTamanho; j++) {
      currentPositionHorario = getPosition(i, j, pCamada);
      currentPositionGrade = getPosition(i, j, 0);

      currentProfessorDisciplina = horario->matriz[currentPositionHorario];

      if (currentProfessorDisciplina != NULL && currentProfessorDisciplina->disciplina == pDisciplina) {
        std::cout << currentPositionHorario << ", ";
        matriz[currentPositionGrade] = currentProfessorDisciplina;
      }

    }
  }

  //std::lower_bound(disciplinasAdicionadas.begin(), disciplinasAdicionadas.end(), pDisciplina, DisciplinaCargaHorariaDesc());
  disciplinasAdicionadas.push_back(pDisciplina);
}

bool Grade::insert(Disciplina* pDisciplina) {
  return insert(pDisciplina, false);
}

bool Grade::insert(Disciplina* pDisciplina, bool force) {
  int camada, triDimensional[3];
  int x;

  std::vector<ProfessorDisciplina*>::iterator pdIter = horario->matriz.begin();
  std::vector<ProfessorDisciplina*>::iterator pdIterEnd = horario->matriz.end();
  std::vector<ProfessorDisciplina*>::iterator pdIterFound = horario->matriz.begin();

  bool viavel = false;
  bool first = true;

  if (std::find(disciplinasAdicionadas.begin(), disciplinasAdicionadas.end(), pDisciplina) == disciplinasAdicionadas.end()) {
    while (((pdIterFound = getFirstDisciplina(pdIterFound, pdIterEnd, pDisciplina)) != pdIterEnd) && (first || !viavel || force)) {
      x = getPositionDisciplina(pdIter, pdIterEnd, pdIterFound);

      professorDisciplinaTemp = NULL;

      get3DMatrix(x, triDimensional);
      camada = triDimensional[2];

      viavel = isViable(pDisciplina, camada);
      if (viavel) {
        std::cout << pDisciplina->id << "[";
        add(pDisciplina, camada);
        std::cout << "]" << std::endl;
      }
      if (viavel || force) {
        if (professorDisciplinaTemp != NULL) {
          professorDisciplinas.push_back(professorDisciplinaTemp);
        }
      }
      if (!viavel && force) {
        if (professorDisciplinaTemp != NULL) {
          problemas.push_back(professorDisciplinaTemp->disciplina->id);
        }
      }

      first = false;
      pdIterFound++;
    }
  }

  return viavel;
}

Disciplina* Grade::remove(Disciplina* pDisciplina) {
  std::vector<Disciplina*>::iterator found;
  Disciplina* rDisciplina = NULL;
  
  int i = 0;
  int x = getFirstDisciplina(pDisciplina, matriz);
  
  std::cout<< "- Removeu: " << pDisciplina->id << std::endl;

  problemas.erase(std::remove(problemas.begin(), problemas.end(), pDisciplina->id), problemas.end());

  while (x >= 0) {

    matriz[x] = NULL;
    x = getFirstDisciplina(pDisciplina, matriz);

    i++;
  }
  if (i > 0) {
    found = std::remove(disciplinasAdicionadas.begin(), disciplinasAdicionadas.end(), pDisciplina);
    if (found != disciplinasAdicionadas.end()) {
      rDisciplina = *(found);
    }
    disciplinasAdicionadas.erase(found, disciplinasAdicionadas.end());
  }

  return rDisciplina;
}

double Grade::getObjectiveFunction() {
  ProfessorDisciplina *professorDisciplina;

  double fo = 0;

  int triDimensional[3];
  int perfil, diaSemana;

  if (problemas.size() > 0) {
    fo = -1;
  } else {
    for (int i = 0; i < size; i++) {
      professorDisciplina = at(i);

      get3DMatrix(i, triDimensional);

      diaSemana = triDimensional[1];
      perfil = triDimensional[2];

      if (professorDisciplina != NULL) {
        fo += ((1) * (alunoPerfil->peso));
      }
    }
  }

  return fo;
}

Grade* Grade::clone() const {
  Grade* g = new Grade(*this);
  
  g->matriz = std::vector<ProfessorDisciplina*>(matriz.begin(), matriz.end());
  g->alocados = std::vector<std::string>(alocados.begin(), alocados.end());
  g->disciplinasAdicionadas = std::vector<Disciplina*>(disciplinasAdicionadas.begin(), disciplinasAdicionadas.end());
  g->problemas = std::vector<std::string>(problemas.begin(), problemas.end());
  
  g->professorDisciplinaTemp = NULL;
  
  return g;
}