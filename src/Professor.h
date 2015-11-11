#ifndef PROFESSOR_H
#define	PROFESSOR_H

#include <string>
#include <cstring>
#include <map>

#include "UUID.h"

class Professor {
  friend class Representacao;
  friend class Grade;
  friend class Horario;
  friend class Solucao;
  friend class Resolucao;
public:
  Professor(std::string pNome);
  Professor(std::string pNome, std::string pId);
  virtual ~Professor();

  std::string getId();

  std::string getNome();
  void setNome(std::string pNome);

  void setCreditoMaximo(int pCreditoMaximo);
  
  void setDiaDisponivel(unsigned int dia);
  void unsetDiaDisponivel(unsigned int dia);
  bool isDiaDisponivel(unsigned int dia);

  void addCompetencia(std::string pDisciplina, double pPeso);
  bool haveCompetencia(std::string pDisciplina);
private:
  std::string id;

  std::string nome;
  int creditoMaximo;

  bool diasDisponiveis[7];
  
  std::map<std::string, double> competencias;

  void init(std::string pNome, std::string pId);

  bool isDiaValido(unsigned int dia);
  void setDiaDisponivel(unsigned int dia, bool value);
};

#endif	/* PROFESSOR_H */

