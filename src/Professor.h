#ifndef PROFESSOR_H
#define	PROFESSOR_H

#include <string>
#include <cstring>
#include <vector>
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
  
  int getNumDisponibilidade() const;
  void setNome(std::string pNome);

  void setCreditoMaximo(int pCreditoMaximo);
  
  void setDiaDisponivel(unsigned int dia, int bloco);
  void unsetDiaDisponivel(unsigned int dia, int bloco);
  bool isDiaDisponivel(unsigned int dia, int bloco);

  void addCompetencia(std::string pDisciplina, double pPeso);
  bool haveCompetencia(std::string pDisciplina);
private:
  std::string id;

  std::string nome;
  int creditoMaximo;

  std::vector<std::vector<bool>> diasDisponiveis;
  int numDisponibilidade;
  
  std::map<std::string, double> competencias;

  void init(std::string pNome, std::string pId);

  bool isDiaValido(unsigned int dia);
  void setDiaDisponivel(unsigned int dia, int bloco, bool value);
};

#endif	/* PROFESSOR_H */

