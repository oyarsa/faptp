#ifndef HORARIO_H
#define	HORARIO_H

#include "Representacao.h"

class Horario : public Representacao {
public:
  Horario(int pBlocosTamanho, int pCamadasTamanho);
  virtual ~Horario();
  
  double getObjectiveFunction();
private:

};

#endif	/* HORARIO_H */

