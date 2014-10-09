#ifndef REPRESENTACAO_H
#define	REPRESENTACAO_H

#include <cmath>
#include <string>
#include <vector>

#include "Semana.h"

#include "ProfessorDisciplina.h"
#include "Util.h"

class Representacao {
public:
  Representacao(int pBlocosTamanho, int pCamadasTamanho);
  virtual ~Representacao();

  ProfessorDisciplina* at(int pBloco, int pDia, int pCamada);
  ProfessorDisciplina* at(int position);
  void insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina);
  void insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force);

  void get3DMatrix(int pLinear, int* triDimensional);

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

