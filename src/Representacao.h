#ifndef REPRESENTACAO_H
#define	REPRESENTACAO_H

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "../template/Algorithms.h"
#include "Semana.h"

#include "ProfessorDisciplina.h"
#include "Util.h"

class Representacao {
  friend class Resolucao;
public:
  Representacao(int pBlocosTamanho, int pCamadasTamanho);
  virtual ~Representacao();

  ProfessorDisciplina* at(int pBloco, int pDia, int pCamada);
  ProfessorDisciplina* at(int position);

  virtual bool insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina);
  virtual bool insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force);

  void get3DMatrix(int pLinear, int* triDimensional);

  int getFirstDisciplina(Disciplina* pDisciplina);
  int getFirstDisciplina(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> pMatriz);

  virtual double getObjectiveFunction() = 0;
protected:
  int blocosTamanho;
  int camadasTamanho;

  int size;

  std::vector<std::string> blocos;
  std::vector<std::string> camadas;

  std::vector<ProfessorDisciplina*> matriz;
  std::vector<std::string> alocados;

  void initMatriz();
  int getPosition(int pBloco, int pDia, int pCamada);
};

#endif	/* REPRESENTACAO_H */

