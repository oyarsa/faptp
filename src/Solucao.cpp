#include <numeric>
#include <functional>

#include "Solucao.h"
#include "Aleatorio.h"

Solucao::Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
    blocosTamanho = pBlocosTamanho;
    camadasTamanho = pCamadasTamanho;
    perfisTamanho = pPerfisTamanho;
    id = aleatorio::randomInt();
    init();
}

Solucao::Solucao(const Solucao& outro) 
    : horario(new Horario(*(outro.horario)))
    , id(aleatorio::randomInt())
    , blocosTamanho(outro.blocosTamanho)
    , camadasTamanho(outro.camadasTamanho)
    , perfisTamanho(outro.perfisTamanho)
    , gradesLength(outro.gradesLength)
	, fo(-1)
{
    for (const auto& par : outro.grades) {
        grades[par.first] = new Grade(*(par.second));
    }
}

Solucao& Solucao::operator=(const Solucao& outro) {
    horario = new Horario(*(outro.horario));
    id = aleatorio::randomInt();
    blocosTamanho = outro.blocosTamanho;
    camadasTamanho = outro.camadasTamanho;
    perfisTamanho = outro.perfisTamanho;
    gradesLength = outro.gradesLength;
	fo = -1;
    
    for (const auto& par : outro.grades) {
        grades[par.first] = new Grade(*(par.second));
    }

	return *this;
}


void Solucao::init() {
    horario = new Horario(blocosTamanho, camadasTamanho);
	fo = -1;
    gradesLength = 0;
}

Solucao::~Solucao() {
    delete horario;
    
    for (auto& par : grades) {
		delete par.second;
    }
}

void Solucao::insertGrade(Grade* grade) {
    auto& alvoInsercao = grades[grade->alunoPerfil->id];
    if (alvoInsercao) {
        delete alvoInsercao;
    } else {
		gradesLength++;
    }
    alvoInsercao = grade;
}

double Solucao::getFO() {
	if (fo == -1) {
		fo = std::accumulate(begin(grades), end(grades), 0.0,
							   [](const double& acc, const std::pair<std::string, Grade*>& par) {
			return acc + par.second->getFO();
		});
	}
	return fo;
}

std::size_t Solucao::getHash() {
	return horario->getHash();
}
