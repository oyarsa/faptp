#ifndef RESOLUCAO_H
#define	RESOLUCAO_H

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <vector>

#include <time.h>

#include "../template/Algorithms.h"

#include "Professor.h"
#include "Disciplina.h"
#include "ProfessorDisciplina.h"
#include "AlunoPerfil.h"
#include "Solucao.h"
#include "Horario.h"

#include "Util.h"
#include "includes/json/json.h"

#include "Files.h"

#define RESOLUCAO_GERAR_GRADE_TIPO_GULOSO          1
#define RESOLUCAO_GERAR_GRADE_TIPO_GRASP           2
#define RESOLUCAO_GERAR_GRADE_TIPO_COMBINATORIO    3

#define RESOLUCAO_GRASP_TEMPO_CONSTRUCAO_FATOR_DEFAULT    2.285714
#define RESOLUCAO_GRASP_ITERACAO_VIZINHOS_DEDAULT         10

#define RESOLUCAO_GRASP_VIZINHOS_ALEATORIOS   1
#define RESOLUCAO_GRASP_VIZINHOS_CRESCENTE    2

class Resolucao {
public:
  Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
  Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho, std::string pSolucaoTxt);
  virtual ~Resolucao();

  void start(int pTipo);
  void start(int pTipo, double x);

  int gerarGrade(int pTipo, double x);
  
  int graspVizinhanca;
  int graspVizinhos;
  float graspTempoConstrucao;
private:
  int blocosTamanho;
  int camadasTamanho;
  int perfisTamanho;

  std::string solucaoTxt;

  std::map<std::string, Professor*> professores;

  std::map<std::string, int> disciplinasIndex;
  std::vector<Disciplina*> disciplinas;

  std::map<std::string, AlunoPerfil*> alunoPerfis;

  std::map<std::string, ProfessorDisciplina*> professorDisciplinas;

  std::vector<Solucao*> solucoes;
  
  Json::Value jsonRoot;

  void init(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
  void initGrasp();

  void carregarDados();

  void carregarDadosProfessores();
  void carregarDadosDisciplinas();

  void carregarAlunoPerfis();

  void carregarDadosProfessorDisciplinas();

  void carregarSolucao();

  std::vector<Disciplina*> ordenarDisciplinas();
  std::vector<Disciplina*> ordenarDisciplinas(std::vector<Disciplina*> pDisciplina);
  void atualizarDisciplinasIndex();

  int gerarGradeTipoGuloso();

  Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, std::vector<Disciplina*> disciplinasRestantes, int maxDeep, int deep, int current);
  Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, std::vector<Disciplina*> disciplinasRestantes, int maxDeep);
  int gerarGradeTipoCombinacaoConstrutiva();
  
  Solucao* gerarGradeTipoGraspConstrucao(Solucao *pSolucao, double alfa);

  Grade* gerarGradeTipoGraspConstrucao(Grade* pGrade, double alfa);
  Grade* gerarGradeTipoGraspConstrucao(Grade* pGrade, double alfa, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
  Solucao* gerarGradeTipoGraspRefinamentoAleatorio(Solucao *pSolucao, double alfa);
  Solucao* gerarGradeTipoGraspRefinamentoCrescente(Solucao *pSolucao);
  int gerarGradeTipoGrasp(double alfa);
  std::vector<Disciplina*>::iterator getLimiteIntervaloGrasp(std::vector<Disciplina*> pApRestante, double alfa);
  int getIntervaloAlfaGrasp(std::vector<Disciplina*> pApRestante, double alfa);
};

#endif	/* RESOLUCAO_H */