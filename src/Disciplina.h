#ifndef DISCIPLINA_H
#define	DISCIPLINA_H

#include <string>
#include <cstring>
#include <algorithm>
#include <vector>

#include "Semana.h"
#include "Professor.h"

#include "UUID.h"

class Disciplina {
  friend class Representacao;
  friend class Grade;
  friend class Horario;
  friend class Solucao;
  friend class Resolucao;
public:
  Disciplina(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso, std::string pTurma, int pCapacidade);
  Disciplina(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade);
  Disciplina(const Disciplina& orig);
  virtual ~Disciplina();

  std::string getId() const;

  std::string getNome();
  void setNome(std::string pNome);

  int getPeriodo();
  void setPeriodo(int pPeriodo);

  std::string getCurso();
  void setCurso(std::string pCurso);

  int getCargaHoraria() const;
  void setCargaHoraria(int pCargaHoraria);

  int getAulasSemana();
  int getCreditos();

  void addPreRequisito(std::string pDisciplina);
  bool isPreRequisito(std::string pDisciplina);
  
  void addProfessorCapacitado(Professor *professor);
private:
  std::string id;
  std::string turma;

  std::string nome;

  int periodo;
  std::string curso;

  int cargaHoraria;
  int aulasSemana;
  int capacidade;
  int alocados;
  bool ofertada;

  std::vector<std::string> preRequisitos;
  std::vector<std::string> equivalentes;

  std::vector<Professor*> professoresCapacitados;

  void init(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade);
};

#endif	/* DISCIPLINA_H */

