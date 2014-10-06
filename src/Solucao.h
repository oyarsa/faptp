#ifndef SOLUCAO_H
#define	SOLUCAO_H

#include "Horario.h"
#include "Grade.h"

class Solucao {
  friend class Resolucao;
public:
  Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
  virtual ~Solucao();

  void insertGrade(Grade* grade);
  void insertGrade(Grade* grade, int position);

  double getObjectiveFunction();
private:
  Horario *horario;
  std::vector<Grade*> grades;

  int blocosTamanho;
  int camadasTamanho;
  int perfisTamanho;

  int gradesLength;

  void init();
};

#endif	/* SOLUCAO_H */

