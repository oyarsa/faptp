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

using namespace std;

class Grade : public Representacao {
  friend class Solucao;
  friend class Resolucao;
public:
  Grade(int pBlocosTamanho, AlunoPerfil *pAlunoPerfil, Horario *pHorario);
  virtual ~Grade();

  bool insert(Disciplina* pDisciplina);
  bool insert(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
  bool insert(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar, bool force);
  Disciplina* remove(Disciplina* pDisciplina, ProfessorDisciplina* &pProfessorDisciplina);
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
  bool checkCollision(Disciplina* pDisciplina, int pCamada, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
  bool isViable(Disciplina* pDisciplina, int pCamada, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
  
  void add(Disciplina* pDisciplina, int pCamada);
};

#endif	/* GRADE_H */

