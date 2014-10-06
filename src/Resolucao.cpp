#include "Resolucao.h"

Resolucao::Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
  blocosTamanho = pBlocosTamanho;
  camadasTamanho = pCamadasTamanho;
  perfisTamanho = pPerfisTamanho;

  init();
}

void Resolucao::init() {
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
    std::cout << "Unable to open file";
    exit(EXIT_FAILURE);
  }
}

void Resolucao::carregarDadosDisciplinas() {
  Util util;

  std::vector<std::string> pieces;
  std::vector<std::string> fPreRequisitos;
  std::string line;

  std::ifstream myfile(TXT_DISCIPLINA);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      disciplinas[pieces[DISCIPLINA_ID]] = new Disciplina(pieces[DISCIPLINA_NOME], atoi(pieces[DISCIPLINA_CARGA_HORARIA].c_str()), atoi(pieces[DISCIPLINA_PERIODO].c_str()), pieces[DISCIPLINA_CURSO], pieces[DISCIPLINA_ID]);

      if (pieces.size() == (DISCIPLINA_PRE_REQUISITO + 1)) {
        fPreRequisitos = util.strSplit(pieces[DISCIPLINA_PRE_REQUISITO], '|');
        disciplinas[pieces[DISCIPLINA_ID]]->preRequisitos = fPreRequisitos;
      }
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file";
    exit(EXIT_FAILURE);
  }
}

void Resolucao::carregarDadosProfessorDisciplinas() {
  Util util;

  ProfessorDisciplina *pd;

  double competenciaPeso;

  std::vector<std::string> pieces;
  std::string line;

  std::ifstream myfile(TXT_PROFESSOR_DISCIPLINA);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      pd = new ProfessorDisciplina(professores[pieces[PROFESSOR_DISCIPLINA_PROFESSOR]], disciplinas[pieces[PROFESSOR_DISCIPLINA_DISCIPLINA]]);

      competenciaPeso = 1.0;
      if (pieces.size() == (PROFESSOR_DISCIPLINA_PESO + 1)) {
        competenciaPeso = atof(pieces[PROFESSOR_DISCIPLINA_PESO].c_str());
      }
      pd->professor->addCompetencia(pieces[1], competenciaPeso);

      professorDisciplinas.push_back(pd);
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file";
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

  std::ifstream myfile(TXT_PROFESSOR);

  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');

      ap = new AlunoPerfil(atof(pieces[ALUNO_PERFIL_PESO].c_str()), pieces[ALUNO_PERFIL_ID]);
      
      fCursadas = util.strSplit(pieces[ALUNO_PERFIL_CURSADAS], '|');
      ap->cursadas = fCursadas;
      
      if (pieces.size() == (ALUNO_PERFIL_RESTANTE + 1)) {
        fRestante = util.strSplit(pieces[ALUNO_PERFIL_RESTANTE], '|');
        ap->restante = fRestante;
      }
      
      alunoPerfis.push_back(ap);
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file";
    exit(EXIT_FAILURE);
  }
}

Resolucao::~Resolucao() {

}

void Resolucao::start() {

}

