#ifndef HORARIO_H
#define	HORARIO_H

#include "Representacao.h"

#define HORARIO_BLOCO                   0
#define HORARIO_DIA                     1
#define HORARIO_CAMADA                  2
#define HORARIO_PROFESSOR_DISCIPLINA    3

class Horario : public Representacao {
public:
  Horario(int pBlocosTamanho, int pCamadasTamanho);
  virtual ~Horario();

  void insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina);
  void insert(int pBloco, int pDia, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force);
  double getObjectiveFunction();
private:

};

#endif	/* HORARIO_H */

