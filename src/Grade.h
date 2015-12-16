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
  Grade(int pBlocosTamanho, AlunoPerfil *pAlunoPerfil, Horario *pHorario,
          const std::vector<Disciplina*>& pDisciplinasCurso, std::map<std::string, int>& pDiscToIndex);
  Grade(const Grade& outro);
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
  
  std::vector<ProfessorDisciplina*> professorDisciplinas;
  std::vector<std::string> problemas;
  ProfessorDisciplina *professorDisciplinaTemp;
  std::vector<Disciplina*> disciplinasAdicionadas;
  
  const std::vector<Disciplina*>& disciplinasCurso;
  std::map<std::string, int>& discToIndex;
  
  //! Recebe um nome e retorna um ponteiro para uma disciplina
  Disciplina* getDisciplina(std::string pNomeDisc);

  void init();
  
  bool havePreRequisitos(const Disciplina* const pDisciplina);
  bool hasPeriodoMinimo(const Disciplina* const pDisciplina);
  bool hasCoRequisitos(const Disciplina* const pDisciplina);
  bool checkCollision(const Disciplina* const pDisciplina, const int pCamada, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar);
  bool isViable(const Disciplina* const pDisciplina, const int pCamada, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar);
  //! Verifica se a disciplina sendo considerada n�o est� na lista de equival�ncias
  //! de outra que j� foi inserida
  bool discRepetida(const Disciplina* const pDisciplina);
  
  void add(Disciplina* pDisciplina, int pCamada);
};

#endif	/* GRADE_H */

