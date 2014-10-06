#ifndef GRADE_H
#define	GRADE_H

#include <vector>

#include "Semana.h"

#include "Representacao.h"
#include "AlunoPerfil.h"

class Grade : public Representacao {
public:
  Grade(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
  virtual ~Grade();

  double getObjectiveFunction();
private:
  int perfisTamanho;
  std::vector<AlunoPerfil*> perfis;

  void init();
};

#endif	/* GRADE_H */

