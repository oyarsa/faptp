#ifndef DISCIPLINA_H
#define	DISCIPLINA_H

#include <string>
#include <vector>
#include <unordered_set>

#include "Professor.h"

class Disciplina {
  friend class Representacao;
  friend class Grade;
  friend class Horario;
  friend class Solucao;
  friend class Resolucao;
  friend class Output;
public:
  Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo, std::string pCurso, std::string pTurma, int pCapacidade, std::string pPeriodoMinimo);
  Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade, std::string pPeriodoMinimo);
  Disciplina(const Disciplina& orig);
  virtual ~Disciplina();

  std::string getId() const;

  int periodoNum() const;
  std::string getNome() const;
  void setNome(std::string pNome);

  std::string getPeriodo();
  void setPeriodo(std::string pPeriodo);

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

  std::string periodo;
  std::string periodoMinimo;
  std::string curso;

  int cargaHoraria;
  int aulasSemana;
  int capacidade;
  int alocados;
  bool ofertada;

  std::unordered_set<std::string> preRequisitos;
  std::unordered_set<std::string> coRequisitos;
  std::unordered_set<std::string> equivalentes;

  std::vector<Professor*> professoresCapacitados;

  void init(std::string pNome, int pCargaHoraria, std::string pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade, std::string pPeriodoMinimo);
};

#endif	/* DISCIPLINA_H */

