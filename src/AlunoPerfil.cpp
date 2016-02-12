#include "AlunoPerfil.h"

AlunoPerfil::AlunoPerfil(double pPeso) {
  UUID uuid;
  std::string pId, pTurma;
  std::string pPeriodo;
  
  pId = uuid.GenerateUuid();
  pTurma = "A";
  pPeriodo = "1";
  
  init(pPeso, pId, pTurma, pPeriodo);
}

AlunoPerfil::AlunoPerfil(double pPeso, std::string pId, std::string pTurma, std::string pPeriodo) {
  init(pPeso, pId, pTurma, pPeriodo);
}

void AlunoPerfil::init(double pPeso, std::string pId, std::string pTurma, std::string pPeriodo) {
  id = pId;
  peso = pPeso;
  turma = pTurma;
  periodo = pPeriodo;
}

AlunoPerfil::~AlunoPerfil() {
}

double AlunoPerfil::getPeso() {
  return peso;
}

void AlunoPerfil::setPeso(double pPeso) {
  peso = pPeso;
}

void AlunoPerfil::addCursada(std::string pCursada) {
  cursadas.push_back(pCursada);
}

bool AlunoPerfil::isCursada(std::string pCursada) {
  return find(cursadas.begin(), cursadas.end(), pCursada) != cursadas.end();
}

void AlunoPerfil::addRestante(Disciplina* pRestante) {
  restante.push_back(pRestante);
}

bool AlunoPerfil::isRestante(Disciplina* pRestante) {
  return find(restante.begin(), restante.end(), pRestante) != restante.end();
}