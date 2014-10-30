#ifndef ALUNOPERFIL_H
#define	ALUNOPERFIL_H

#include <string>
#include <algorithm>
#include <vector>

#include "Disciplina.h"
#include "UUID.h"

#define ALUNO_PERFIL_ID         0
#define ALUNO_PERFIL_PESO       1
#define ALUNO_PERFIL_RESTANTE   2
#define ALUNO_PERFIL_CURSADAS   3

class AlunoPerfil {
  friend class Grade;
  friend class Resolucao;
public:
  AlunoPerfil(double pPeso);
  AlunoPerfil(double pPeso, std::string pId);
  virtual ~AlunoPerfil();
  
  double getPeso();
  void setPeso(double pPeso);
  
  void addCursada(std::string pCursada);
  bool isCursada(std::string pCursada);
  
  void addRestante(Disciplina* pRestante);
  bool isRestante(Disciplina* pRestante);
private:
  std::string id;
  
  double peso;
  
  std::vector<std::string> cursadas;
  std::vector<Disciplina*> restante;
  
  void init(double pPeso, std::string pId);
};

#endif	/* ALUNOPERFIL_H */

