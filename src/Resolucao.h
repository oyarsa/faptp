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
#include "Util.h"

#include "Files.h"

class Resolucao {
public:
  Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
  virtual ~Resolucao();

  void start();
private:
  int blocosTamanho;
  int camadasTamanho;
  int perfisTamanho;
  
  std::map<std::string, Professor*> professores;
  std::map<std::string, Disciplina*> disciplinas;

  std::vector<AlunoPerfil*> alunoPerfis;

  std::vector<ProfessorDisciplina*> professorDisciplinas;
  
  std::vector<Solucao*> solucoes;
  
  void init();

  void carregarDados();

  void carregarDadosProfessores();
  void carregarDadosDisciplinas();

  void carregarAlunoPerfis();

  void carregarDadosProfessorDisciplinas();
};

#endif	/* RESOLUCAO_H */

