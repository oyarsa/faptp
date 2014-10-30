#include "AlunoPerfil.h"

AlunoPerfil::AlunoPerfil(double pPeso) {
  UUID uuid;
  std::string pId;
  
  pId = uuid.GenerateUuid();
  
  init(pPeso, pId);
}

AlunoPerfil::AlunoPerfil(double pPeso, std::string pId) {
  init(pPeso, pId);
}

void AlunoPerfil::init(double pPeso, std::string pId) {
  id = pId;
  peso = pPeso;
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
  return std::find(cursadas.begin(), cursadas.end(), pCursada) != cursadas.end();
}

void AlunoPerfil::addRestante(Disciplina* pRestante) {
  restante.push_back(pRestante);
}

bool AlunoPerfil::isRestante(Disciplina* pRestante) {
  return std::find(restante.begin(), restante.end(), pRestante) != restante.end();
}