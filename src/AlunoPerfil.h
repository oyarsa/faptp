#ifndef ALUNOPERFIL_H
#define	ALUNOPERFIL_H

#include <string>
#include <unordered_set>
#include <set>

#include "Disciplina.h"

class AlunoPerfil
{
	friend class Grade;
	friend class Solucao;
	friend class Resolucao;
	friend class Output;
public:
	AlunoPerfil(double pPeso, const std::string& pId, const std::string& pTurma, const std::string& pPeriodo);
	virtual ~AlunoPerfil();

	double getPeso() const;
	long long getHash();
	void setPeso(double pPeso);

	void addCursada(const std::string& pCursada);
	bool isCursada(const std::string& pCursada);

	void addRestante(Disciplina* pRestante);
	bool isRestante(const std::string& pRestante);
private:

	struct DisciplinaCargaHorariaDesc
	{
		bool operator()(const Disciplina* a, const Disciplina* b)
		{
			return a->getCargaHoraria() > b->getCargaHoraria();
		}
	};

	std::string id;
	std::string turma;
	std::string periodo;

	double peso;
	long long hash;

	std::unordered_set<std::string> cursadas;
	std::unordered_set<std::string> aprovadas;
	//std::unordered_set<std::string> restante;
	std::vector<std::string> restante;
	std::vector<Disciplina*> restanteOrd;
};

#endif /* ALUNOPERFIL_H */

