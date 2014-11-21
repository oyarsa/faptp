#ifndef GRADE_H
#define	GRADE_H

#include <algorithm>
#include <string>
#include <vector>

#include "Semana.h"

#include "Representacao.h"
#include "Horario.h"
#include "AlunoPerfil.h"
#include "Disciplina.h"
#include "ProfessorDisciplina.h"

class Grade : public Representacao {
  friend class Solucao;
  friend class Resolucao;
public:
  Grade(int pBlocosTamanho, AlunoPerfil *pAlunoPerfil, Horario *pHorario);
  virtual ~Grade();

  bool insert(Disciplina* pDisciplina);
  bool insert(Disciplina* pDisciplina, bool force);
  Disciplina* remove(Disciplina* pDisciplina);

  double getObjectiveFunction();
  
  Grade* clone() const;
private:
  AlunoPerfil *alunoPerfil;
  Horario *horario;
  
  int fo;
  std::vector<ProfessorDisciplina*> professorDisciplinas;
  std::vector<std::string> problemas;
  ProfessorDisciplina *professorDisciplinaTemp;
  std::vector<Disciplina*> disciplinasAdicionadas;

  void init();
  
  bool havePreRequisitos(Disciplina *pDisciplina);
  bool checkCollision(Disciplina* pDisciplina, int pCamada);
  bool isViable(Disciplina* pDisciplina, int pCamada);
  
  void add(Disciplina* pDisciplina, int pCamada);
};

#endif	/* GRADE_H */

