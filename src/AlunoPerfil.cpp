#include <utility>
#include <sstream>

#include "AlunoPerfil.h"
#include "Util.h"

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
	oss << turma << periodo;
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

void AlunoPerfil::addRestante(Disciplina* pRestante) {
  //restante.insert(pRestante->getId());
	restante.push_back(pRestante->getId());
  Util::insert_sorted(restanteOrd, pRestante, DisciplinaCargaHorariaDesc{});
}

bool AlunoPerfil::isRestante(const std::string& pRestante) {
  //return restante.find(pRestante) != restante.end();
	return find(begin(restante), end(restante), pRestante) != end(restante);
}