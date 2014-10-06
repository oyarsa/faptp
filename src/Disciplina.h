#ifndef DISCIPLINA_H
#define	DISCIPLINA_H

#include <string>
#include <cstring>
#include <algorithm>
#include <vector>

#include "UUID.h"

#define DISCIPLINA_ID             0
#define DISCIPLINA_NOME           1
#define DISCIPLINA_CARGA_HORARIA  2
#define DISCIPLINA_PERIODO        3
#define DISCIPLINA_CURSO          4
#define DISCIPLINA_PRE_REQUISITO  5

class Disciplina {
  friend class Representacao;
  friend class Grade;
  friend class Horario;
  friend class Solucao;
  friend class Resolucao;
public:
  Disciplina(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso);
  Disciplina(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso, std::string pId);
  Disciplina(const Disciplina& orig);
  virtual ~Disciplina();

  std::string getId();

  std::string getNome();
  void setNome(std::string pNome);

  int getPeriodo();
  void setPeriodo(int pPeriodo);

  std::string getCurso();
  void setCurso(std::string pCurso);

  int getCargaHoraria();
  void setCargaHoraria(int pCargaHoraria);

  void addPreRequisito(std::string pDisciplina);
  bool isPreRequisito(std::string pDisciplina);
private:
  std::string id;

  std::string nome;

  int periodo;
  std::string curso;

  int cargaHoraria;


  std::vector<std::string> preRequisitos;

  void init(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso, std::string pId);
};

#endif	/* DISCIPLINA_H */

