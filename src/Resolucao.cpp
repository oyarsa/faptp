#include "Resolucao.h"

Resolucao::Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
  init(pBlocosTamanho, pCamadasTamanho, pPerfisTamanho);
}

Resolucao::Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho, std::string pSolucaoTxt) {
  solucaoTxt = pSolucaoTxt;

  init(pBlocosTamanho, pCamadasTamanho, pPerfisTamanho);
}

void Resolucao::init(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
  blocosTamanho = pBlocosTamanho;
  camadasTamanho = pCamadasTamanho;
  perfisTamanho = pPerfisTamanho;

  carregarDados();
}

void Resolucao::carregarDados() {
  carregarDadosProfessores();
  carregarDadosDisciplinas();
  carregarDadosProfessorDisciplinas();
  carregarAlunoPerfis();
}

void Resolucao::carregarDadosProfessores() {
  Util util;

  std::vector<std::string> pieces;
  std::string line;

  std::ifstream myfile(TXT_PROFESSOR);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      professores[pieces[PROFESSOR_ID]] = new Professor(pieces[PROFESSOR_NOME], pieces[PROFESSOR_ID]);
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file " << TXT_PROFESSOR;
    exit(EXIT_FAILURE);
  }
}

void Resolucao::carregarDadosDisciplinas() {
  Util util;

  Disciplina *disciplina;

  std::vector<std::string> pieces;
  std::vector<std::string> fPreRequisitos;
  std::string line;

  std::ifstream myfile(TXT_DISCIPLINA);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      disciplina = new Disciplina(pieces[DISCIPLINA_NOME], atoi(pieces[DISCIPLINA_CARGA_HORARIA].c_str()), atoi(pieces[DISCIPLINA_PERIODO].c_str()), pieces[DISCIPLINA_CURSO], pieces[DISCIPLINA_ID]);

      if (pieces.size() == (DISCIPLINA_PRE_REQUISITO + 1)) {
        fPreRequisitos = util.strSplit(pieces[DISCIPLINA_PRE_REQUISITO], '|');
        disciplina->preRequisitos = fPreRequisitos;
      }

      disciplinas.push_back(disciplina);
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file " << TXT_DISCIPLINA;
    exit(EXIT_FAILURE);
  }

  disciplinas = ordenarDisciplinas();
}

void Resolucao::carregarDadosProfessorDisciplinas() {
  Util util;

  ProfessorDisciplina *professorDisciplina;

  double competenciaPeso;

  std::vector<std::string> pieces;
  std::string line;

  std::ifstream myfile(TXT_PROFESSOR_DISCIPLINA);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      professorDisciplina = new ProfessorDisciplina(professores[pieces[PROFESSOR_DISCIPLINA_PROFESSOR]], disciplinas[disciplinasIndex[pieces[PROFESSOR_DISCIPLINA_DISCIPLINA]]], pieces[PROFESSOR_DISCIPLINA_ID]);

      competenciaPeso = 1.0;
      if (pieces.size() == (PROFESSOR_DISCIPLINA_PESO + 1)) {
        competenciaPeso = atof(pieces[PROFESSOR_DISCIPLINA_PESO].c_str());
      }
      professorDisciplina->professor->addCompetencia(pieces[PROFESSOR_DISCIPLINA_DISCIPLINA], competenciaPeso);

      professorDisciplinas[pieces[PROFESSOR_DISCIPLINA_ID]] = professorDisciplina;
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file " << TXT_PROFESSOR_DISCIPLINA;
    exit(EXIT_FAILURE);
  }
}

void Resolucao::carregarAlunoPerfis() {
  Util util;

  AlunoPerfil *alunoPerfil;

  std::vector<std::string> pieces;
  std::vector<std::string> fStringCursadas;
  std::vector<std::string> fStringRestante;
  std::string line;

  Disciplina *disciplina;

  std::ifstream myfile(TXT_ALUNO_PERFIL);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      alunoPerfil = new AlunoPerfil(atof(pieces[ALUNO_PERFIL_PESO].c_str()), pieces[ALUNO_PERFIL_ID]);

      fStringRestante = util.strSplit(pieces[ALUNO_PERFIL_RESTANTE], '|');
      for (int i = 0; i < fStringRestante.size(); i++) {
        disciplina = disciplinas[disciplinasIndex[fStringRestante[i]]];
        alunoPerfil->addRestante(disciplina);
      }
      alunoPerfil->restante = ordenarDisciplinas(alunoPerfil->restante);

      if (pieces.size() == (ALUNO_PERFIL_CURSADAS + 1)) {
        fStringCursadas = util.strSplit(pieces[ALUNO_PERFIL_CURSADAS], '|');
        alunoPerfil->cursadas = fStringCursadas;
      }

      alunoPerfis[pieces[ALUNO_PERFIL_ID]] = alunoPerfil;
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file";
    exit(EXIT_FAILURE);
  }
}

Resolucao::~Resolucao() {

}

void Resolucao::start(int pTipo, double x) {
  if (solucaoTxt != "") {
    carregarSolucao();
  }

  gerarGrade(pTipo, x);
}

void Resolucao::carregarSolucao() {
  Util util;

  Solucao *solucao = new Solucao(blocosTamanho, camadasTamanho, perfisTamanho);

  ProfessorDisciplina *professorDisciplina;

  int bloco, dia, camada;

  std::vector<std::string> pieces;
  std::string line;

  std::ifstream myfile(solucaoTxt.c_str());

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      bloco = atoi(pieces[HORARIO_BLOCO].c_str());
      dia = atoi(pieces[HORARIO_DIA].c_str());
      camada = atoi(pieces[HORARIO_CAMADA].c_str());

      professorDisciplina = professorDisciplinas[pieces[HORARIO_PROFESSOR_DISCIPLINA]];
      std::cout << "D:" << dia << " - B:" << bloco << " - C:" << camada << " - DSP:" << professorDisciplina->disciplina->nome << "  - P:";
      solucao->horario->insert(dia, bloco, camada, professorDisciplina);
    }
    std::cout << "-----------------------------------------" << std::endl;
    myfile.close();
  } else {
    std::cout << "Unable to open file";
    exit(EXIT_FAILURE);
  }

  solucoes.push_back(solucao);
}

std::vector<Disciplina*> Resolucao::ordenarDisciplinas() {
  disciplinas = ordenarDisciplinas(disciplinas);

  atualizarDisciplinasIndex();

  return disciplinas;
}

std::vector<Disciplina*> Resolucao::ordenarDisciplinas(std::vector<Disciplina*> pDisciplinas) {
  std::vector<Disciplina*>::iterator dIter = pDisciplinas.begin();
  std::vector<Disciplina*>::iterator dIterEnd = pDisciplinas.end();

  std::sort(dIter, dIterEnd, DisciplinaCargaHorariaDesc());

  return pDisciplinas;
}

void Resolucao::atualizarDisciplinasIndex() {
  Disciplina *disciplina;

  std::vector<Disciplina*>::iterator dIter = disciplinas.begin();
  std::vector<Disciplina*>::iterator dIterEnd = disciplinas.end();

  disciplinasIndex.clear();
  for (int i = 0; dIter != dIterEnd; ++dIter, i++) {
    disciplina = *dIter;
    disciplinasIndex[disciplina->id] = i;
  }
}

int Resolucao::gerarGrade(int pTipo, double x) {
  switch (pTipo) {
    case RESOLUCAO_GERAR_GRADE_TIPO_GULOSO:

      return gerarGradeTipoGuloso();

      break;

    case RESOLUCAO_GERAR_GRADE_TIPO_GRASP:

      return gerarGradeTipoGrasp(x);

      break;
  }

  return 0;
}

int Resolucao::gerarGradeTipoGuloso() {
  std::vector<Solucao*>::iterator sIter = solucoes.begin();
  std::vector<Solucao*>::iterator sIterEnd = solucoes.end();
  Solucao *solucao;

  Horario *horario;
  Grade *apGrade;

  std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
  std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();
  AlunoPerfil *alunoPerfil;

  std::vector<Disciplina*>::iterator dIter;
  std::vector<Disciplina*>::iterator dIterEnd;
  Disciplina *disciplina;
  std::vector<Disciplina*> apRestante;
  std::vector<std::string> apCursadas;

  for (; sIter != sIterEnd; ++sIter) {
    solucao = *sIter;
    horario = solucao->horario;

    for (; apIter != apIterEnd; ++apIter) {
      std::cout << apIter->first << std::endl;
      alunoPerfil = alunoPerfis[apIter->first];

      apGrade = new Grade(blocosTamanho, alunoPerfil, horario);

      apRestante = alunoPerfil->restante;
      apCursadas = alunoPerfil->cursadas;

      dIter = apRestante.begin();
      dIterEnd = apRestante.end();

      for (; dIter != dIterEnd; ++dIter) {
        disciplina = *dIter;

        apGrade->insert(disciplina);
      }

      solucao->insertGrade(apGrade);
    }

    std::cout << solucao->getObjectiveFunction();
  }
}

Solucao* Resolucao::gerarGradeTipoGraspConstrucao(Solucao* pSolucao, double alpha) {
  Horario *horario;
  Grade *apGrade;

  std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
  std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();
  AlunoPerfil *alunoPerfil;

  std::vector<std::string> apCursadas;
  std::vector<Disciplina*> apRestante;
  std::vector<Disciplina*>::iterator dIter;
  std::vector<Disciplina*>::iterator dIterLimit;
  std::vector<Disciplina*>::iterator dIterEnd;
  std::vector<Disciplina*>::iterator current;
  Disciplina *disciplina;

  Util util;

  int adicionados;
  int disponivel = (SEMANA - 2) * camadasTamanho;

  horario = pSolucao->horario;

  for (; apIter != apIterEnd; ++apIter) {
    std::cout << apIter->first << std::endl;
    alunoPerfil = alunoPerfis[apIter->first];

    apGrade = new Grade(blocosTamanho, alunoPerfil, horario);

    apCursadas = alunoPerfil->cursadas;
    apRestante = alunoPerfil->restante;

    dIter = apRestante.begin();
    dIterLimit = dIter + (apRestante.size() * alpha);
    dIterEnd = apRestante.end();

    adicionados = 0;
    while (apRestante.size() != 0 && dIter != dIterEnd && disponivel != adicionados) {

      current = dIter + util.randomBetween(0, std::distance(dIter, dIterLimit));
      disciplina = *current;

      if (apGrade->insert(disciplina)) {
        adicionados++;
      }

      apRestante.erase(current);

      dIter = apRestante.begin();
      dIterLimit = dIter + (apRestante.size() * alpha);
    }

    pSolucao->insertGrade(apGrade);
  }

  return pSolucao;
}

Solucao* Resolucao::gerarGradeTipoGraspRefinamento(Solucao* bestSolucao) {
  Solucao *currentSolucao;

  std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
  std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();
  AlunoPerfil *alunoPerfil;

  Util util;

  int random;
  double bestFO, currentFO;

  bestFO = bestSolucao->getObjectiveFunction();

  for (; apIter != apIterEnd; ++apIter) {
    alunoPerfil = alunoPerfis[apIter->first];

    for (int i = 0; i < 5; i++) {
      currentSolucao = bestSolucao->clone();

      random = util.randomBetween(0, currentSolucao->grades[alunoPerfil->id]->disciplinasAdicionadas.size());

      currentFO = currentSolucao->getObjectiveFunction();
      std::cout << "[[V]]-->(" << bestFO << " < " << currentFO << ")" << std::endl;
      if (bestFO < currentFO) {
        bestSolucao = currentSolucao;
        bestFO = currentFO;
        i = 0;
      }
    }
  }

  return bestSolucao;
}

int Resolucao::gerarGradeTipoGrasp(double alpha) {
  std::vector<Solucao*>::iterator sIter = solucoes.begin();
  std::vector<Solucao*>::iterator sIterEnd = solucoes.end();
  Solucao *solucao;
  Solucao *currentSolucao;
  Solucao *bestSolucao;

  double bestFO, currentFO;

  Util util;

  clock_t t0;
  double diff = 0;

  for (; sIter != sIterEnd; ++sIter) {
    solucao = *sIter;
    bestSolucao = solucao->clone();
    bestFO = 0;

    while (diff <= RESOLUCAO_GRASP_TEMPO_CONSTRUCAO) {
      currentSolucao = solucao->clone();

      t0 = clock();

      gerarGradeTipoGraspConstrucao(currentSolucao, alpha);
      gerarGradeTipoGraspRefinamento(currentSolucao);

      diff += util.timeDiff(clock(), t0);

      currentFO = currentSolucao->getObjectiveFunction();
      std::cout << "-->(" << bestFO << " < " << currentFO << ")" << std::endl;
      if (bestFO < currentFO) {
        bestSolucao = currentSolucao;
        bestFO = currentFO;
      }
      std::cout << "-------------------------------------------------" << std::endl;
    }

    std::cout << bestSolucao->getObjectiveFunction() << std::endl;
  }
}
