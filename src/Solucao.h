#ifndef SOLUCAO_H
#define	SOLUCAO_H

#include <string>
#include <map>

#include "Horario.h"
#include "Grade.h"

class Solucao {
  friend class Resolucao;
public:
  Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
  virtual ~Solucao();

  void insertGrade(Grade* grade);

  double getObjectiveFunction();
  
  Solucao* clone() const;
private:
  Horario *horario;
  std::map<std::string, Grade*> grades;

  int blocosTamanho;
  int camadasTamanho;
  int perfisTamanho;

  int gradesLength;

  void init();
};

#endif	/* SOLUCAO_H */

