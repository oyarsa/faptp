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
  friend class Output;
public:
  Grade(int pBlocosTamanho, AlunoPerfil *pAlunoPerfil, Horario *pHorario,
          vector<Disciplina*>& pDisciplinasCurso, map<string, int>& pDiscToIndex);
  Grade(const Grade& outro);
  Grade& operator=(const Grade& outro);
  virtual ~Grade();

  bool insert(Disciplina* pDisciplina);
  bool insert(Disciplina* pDisciplina, vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
  bool insert(Disciplina* pDisciplina, vector<ProfessorDisciplina*> professorDisciplinasIgnorar, bool force);
  Disciplina* remove(Disciplina* pDisciplina, ProfessorDisciplina* &pProfessorDisciplina);
  Disciplina* remove(Disciplina* pDisciplina);

  double getObjectiveFunction();
  
private:
  AlunoPerfil *alunoPerfil;
  Horario *horario;
  
  vector<ProfessorDisciplina*> professorDisciplinas;
  vector<string> problemas;
  ProfessorDisciplina *professorDisciplinaTemp;
  vector<Disciplina*> disciplinasAdicionadas;
  
  vector<Disciplina*>& disciplinasCurso;
  map<string, int>& discToIndex;
  
  //! Recebe um nome e retorna um ponteiro para uma disciplina
  Disciplina* getDisciplina(string pNomeDisc);

  bool havePreRequisitos(const Disciplina* const pDisciplina);
  bool hasPeriodoMinimo(const Disciplina* const pDisciplina);
  bool hasCoRequisitos(const Disciplina* const pDisciplina);
  bool checkCollision(const Disciplina* const pDisciplina, const int pCamada, const vector<ProfessorDisciplina*>& professorDisciplinasIgnorar);
  bool isViable(const Disciplina* const pDisciplina, const int pCamada, const vector<ProfessorDisciplina*>& professorDisciplinasIgnorar);
  //! Verifica se a disciplina sendo considerada não está na lista de equivalências
  //! de outra que já foi inserida
  bool discRepetida(const Disciplina* const pDisciplina);
  
  void add(Disciplina* pDisciplina, int pCamada);
};

#endif	/* GRADE_H */

