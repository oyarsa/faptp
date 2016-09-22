#include <numeric>
#include <functional>

#include "gsl/gsl"

#include "Solucao.h"
#include "Aleatorio.h"

Solucao::Solucao(
    int pBlocosTamanho, 
    int pCamadasTamanho, 
    int pPerfisTamanho
)
    : id(aleatorio::randomInt())
    , blocosTamanho(pBlocosTamanho)
    , camadasTamanho(pCamadasTamanho)
    , perfisTamanho(pPerfisTamanho)
    , horario(std::make_unique<Horario>(blocosTamanho, camadasTamanho))
    , grades()
    , gradesLength(0) {}

Solucao::Solucao(const Solucao& outro)
    : camada_periodo(outro.camada_periodo)
      , id(aleatorio::randomInt())
      , blocosTamanho(outro.blocosTamanho)
      , camadasTamanho(outro.camadasTamanho)
      , perfisTamanho(outro.perfisTamanho)
      , horario(std::make_unique<Horario>(*outro.horario))
      , grades()
      , gradesLength(outro.gradesLength)
      , fo(outro.fo)
{
    for (auto& par : outro.grades) {
        grades[par.first] = new Grade(*par.second);
    }
}

Solucao::~Solucao()
{
    for (auto& par : grades) {
        delete par.second;
    }
}

void Solucao::insertGrade(Grade* grade)
{
    auto& alvoInsercao = grades[grade->aluno->id];
    if (alvoInsercao) {
        delete alvoInsercao;
    } else {
        gradesLength++;
    }
    alvoInsercao = grade;
}

void Solucao::calculaFO()
{
    fo = std::accumulate(begin(grades), end(grades), 0, [](int acc, auto el) {
        return acc + gsl::narrow_cast<int>(el.second->getFO());
    });
}

int Solucao::getFO()
{
    if (fo == -1) {
        calculaFO();
    }
    return fo;
}

int Solucao::getFO() const
{
    if (fo == -1) {
        throw std::runtime_error{"FO não calculada"};
    }
    return fo;
}

std::size_t Solucao::getHash()
{
    return horario->getHash();
}

