#ifndef RESOLUCAO_H
#define	RESOLUCAO_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <vector>

#include "Professor.h"
#include "Disciplina.h"
#include "ProfessorDisciplina.h"
#include "AlunoPerfil.h"
#include "Solucao.h"
#include "Horario.h"

#include "Util.h"

#include "Files.h"

#define RESOLUCAO_GERAR_GRADE_TIPO_GULOSO   1

class Resolucao {
public:
  Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
  Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho, std::string pSolucaoTxt);
  virtual ~Resolucao();

  void start();
  
  int gerarGrade(int pTipo);
private:
  int blocosTamanho;
  int camadasTamanho;
  int perfisTamanho;
  
  std::string solucaoTxt;
  
  std::map<std::string, Professor*> professores;
  std::map<std::string, Disciplina*> disciplinas;

  std::map<std::string, AlunoPerfil*> alunoPerfis;

  std::map<std::string, ProfessorDisciplina*> professorDisciplinas;
  
  std::vector<Solucao*> solucoes;
  
  void init(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);

  void carregarDados();

  void carregarDadosProfessores();
  void carregarDadosDisciplinas();

  void carregarAlunoPerfis();

  void carregarDadosProfessorDisciplinas();
  
  void carregarSolucao();
  
  int gerarGradeTipoGuloso();
};

#endif	/* RESOLUCAO_H */