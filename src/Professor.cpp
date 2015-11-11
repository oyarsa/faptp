#include "Professor.h"

Professor::Professor(std::string pNome) {
  UUID uuid;
  std::string pId;

  pId = uuid.GenerateUuid();

  init(pNome, pId);
}

Professor::Professor(std::string pNome, std::string pId) {
  init(pNome, pId);
}

void Professor::init(std::string pNome, std::string pId) {
  setNome(pNome);
  id = pId;
}

Professor::~Professor() {
}

std::string Professor::getId() {
  return id;
}

std::string Professor::getNome() {
  return nome;
}

void Professor::setNome(std::string pNome) {
  nome = pNome;
}

bool Professor::isDiaValido(unsigned int dia) {
  return (dia < 7);
}

void Professor::setCreditoMaximo(int pCreditoMaximo) {
    creditoMaximo = pCreditoMaximo;
}

void Professor::setDiaDisponivel(unsigned int dia, bool value) {
  if (isDiaValido(dia)) {
    diasDisponiveis[dia] = value;
  }
}

void Professor::setDiaDisponivel(unsigned int dia) {
  setDiaDisponivel(dia, true);
}

void Professor::unsetDiaDisponivel(unsigned int dia) {
  setDiaDisponivel(dia, false);
}

bool Professor::isDiaDisponivel(unsigned int dia) {
  if (isDiaValido(dia)) {
    return diasDisponiveis[dia];
  }

  return false;
}

void Professor::addCompetencia(std::string pDisciplina, double pPeso) {
  competencias[pDisciplina] = pPeso;
}

bool Professor::haveCompetencia(std::string pDisciplina) {
  return competencias.count(pDisciplina);
}