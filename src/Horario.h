#ifndef HORARIO_H
#define	HORARIO_H

#include "Representacao.h"

class Horario : public Representacao {
  friend class Resolucao;
  friend class Grade;
public:
  Horario(int pBlocosTamanho, int pCamadasTamanho);
  virtual ~Horario();

  bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina);
  bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force);

  double getObjectiveFunction();
private:

};

#endif	/* HORARIO_H */

