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
  
  ordenarDisciplinas();
}

void Resolucao::carregarDadosProfessorDisciplinas() {
  Util util;

  double competenciaPeso;

  std::vector<std::string> pieces;
  std::string line;

  std::ifstream myfile(TXT_PROFESSOR_DISCIPLINA);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      professorDisciplinas[pieces[PROFESSOR_DISCIPLINA_ID]] = new ProfessorDisciplina(professores[pieces[PROFESSOR_DISCIPLINA_PROFESSOR]], disciplinas[disciplinasIndex[pieces[PROFESSOR_DISCIPLINA_DISCIPLINA]]]);

      competenciaPeso = 1.0;
      if (pieces.size() == (PROFESSOR_DISCIPLINA_PESO + 1)) {
        competenciaPeso = atof(pieces[PROFESSOR_DISCIPLINA_PESO].c_str());
      }
      professorDisciplinas[pieces[PROFESSOR_DISCIPLINA_ID]]->professor->addCompetencia(pieces[PROFESSOR_DISCIPLINA_DISCIPLINA], competenciaPeso);
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file " << TXT_PROFESSOR_DISCIPLINA;
    exit(EXIT_FAILURE);
  }
}

void Resolucao::carregarAlunoPerfis() {
  Util util;

  AlunoPerfil *ap;

  std::vector<std::string> pieces;
  std::vector<std::string> fCursadas;
  std::vector<std::string> fRestante;
  std::string line;

  std::ifstream myfile(TXT_ALUNO_PERFIL);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      ap = new AlunoPerfil(atof(pieces[ALUNO_PERFIL_PESO].c_str()), pieces[ALUNO_PERFIL_ID]);

      fRestante = util.strSplit(pieces[ALUNO_PERFIL_RESTANTE], '|');
      ap->restante = fRestante;

      if (pieces.size() == (ALUNO_PERFIL_CURSADAS + 1)) {
        fCursadas = util.strSplit(pieces[ALUNO_PERFIL_CURSADAS], '|');
        ap->cursadas = fCursadas;
      }

      alunoPerfis[pieces[ALUNO_PERFIL_ID]] = ap;
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file";
    exit(EXIT_FAILURE);
  }
}

Resolucao::~Resolucao() {

}

void Resolucao::start(int pTipo) {
  if (solucaoTxt != "") {
    carregarSolucao();
  }

  gerarGrade(pTipo);
}

void Resolucao::carregarSolucao() {
  Util util;

  Solucao *solucao = new Solucao(blocosTamanho, camadasTamanho, perfisTamanho);

  std::vector<std::string> pieces;
  std::string line;

  std::ifstream myfile(solucaoTxt.c_str());

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      solucao->horario->insert(atoi(pieces[HORARIO_BLOCO].c_str()), atoi(pieces[HORARIO_DIA].c_str()), atoi(pieces[HORARIO_CAMADA].c_str()), professorDisciplinas[pieces[HORARIO_PROFESSOR_DISCIPLINA]]);
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file";
    exit(EXIT_FAILURE);
  }

  solucoes.push_back(solucao);
}

void Resolucao::ordenarDisciplinas() {
  std::vector<Disciplina*>::iterator dIter = disciplinas.begin();
  std::vector<Disciplina*>::iterator dIterEnd = disciplinas.begin();

  std::sort(dIter, dIterEnd, DisciplinaCargaHorariaDesc());
  
  disciplinasIndex.clear();
  for (int i = 0; dIter != dIterEnd; ++dIter, i++) {
    disciplinasIndex[(*dIter)->id] = i;
  }
}

int Resolucao::gerarGrade(int pTipo) {
  switch (pTipo) {
    case RESOLUCAO_GERAR_GRADE_TIPO_GULOSO:

      return gerarGradeTipoGuloso();

      break;
  }

  return 0;
}

int Resolucao::gerarGradeTipoGuloso() {
  Util util;

  std::vector<Solucao*>::iterator sIter = solucoes.begin();
  std::vector<Solucao*>::iterator sEndIter = solucoes.end();
  Solucao *solucao;
  Horario *horario;

  Grade *apGrade;

  std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
  std::map<std::string, AlunoPerfil*>::iterator apEndIter = alunoPerfis.end();
  AlunoPerfil *alunoPerfil;

  int i, triDimensional[3];
  int bloco, diaSemana, perfil;

  std::vector<std::string> apRestante;
  std::vector<std::string> apCursadas;

  for (; sIter != sEndIter; ++sIter) {
    solucao = *sIter;
    horario = solucao->horario;

    for (; apIter != apEndIter; ++apIter) {
      alunoPerfil = alunoPerfis[apIter->first];

      apGrade = new Grade(blocosTamanho, camadasTamanho, alunoPerfil);

      apRestante = alunoPerfil->restante;
      apCursadas = alunoPerfil->cursadas;

      for (i = 0; i < horario->size; i++) {
        util.get3DMatrix(i, triDimensional, horario->blocosTamanho, horario->camadasTamanho);

        bloco = triDimensional[0];
        diaSemana = triDimensional[1];
        perfil = triDimensional[2];

        apGrade->insert(bloco, diaSemana, perfil, horario);
      }
    }
  }
}