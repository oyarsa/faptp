#include <utility>
#include <sstream>

#include "AlunoPerfil.h"

AlunoPerfil::AlunoPerfil(double pPeso, const std::string& pId, 
						 const std::string& pTurma, const std::string& pPeriodo) 
	: id(pId), turma(pTurma), periodo(pPeriodo), peso(pPeso), hash()
{
}

AlunoPerfil::~AlunoPerfil() {
}

double AlunoPerfil::getPeso() const
{
  return peso;
}

long long AlunoPerfil::getHash()
{
	if (hash != 0) {
		return hash;
	}

	std::ostringstream oss;
	for (auto& disc : aprovadas) {
		oss << disc;
	}
	for (auto& disc : cursadas) {
		oss << disc;
	}

	hash = std::hash<std::string>{}(oss.str());
	return hash;
}

void AlunoPerfil::setPeso(double pPeso) {
  peso = pPeso;
}

void AlunoPerfil::addCursada(const std::string& pCursada) {
  cursadas.insert(pCursada);
}

bool AlunoPerfil::isCursada(const std::string& pCursada) {
  return find(cursadas.begin(), cursadas.end(), pCursada) != cursadas.end();
}

void AlunoPerfil::addRestante(const std::string& pRestante) {
  restante.insert(pRestante);
}

bool AlunoPerfil::isRestante(const std::string& pRestante) {
  return find(restante.begin(), restante.end(), pRestante) != restante.end();
}