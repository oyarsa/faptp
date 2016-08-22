#include <numeric>
#include <functional>

#include <gsl.h>

#include "Solucao.h"
#include "Aleatorio.h"

Solucao::Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho)
{
    blocosTamanho = pBlocosTamanho;
    camadasTamanho = pCamadasTamanho;
    perfisTamanho = pPerfisTamanho;
    id = aleatorio::randomInt();
    init();
}

Solucao::Solucao(const Solucao& outro)
    : camada_periodo(outro.camada_periodo)
      , horario(new Horario(*(outro.horario)))
      , id(aleatorio::randomInt())
      , blocosTamanho(outro.blocosTamanho)
      , camadasTamanho(outro.camadasTamanho)
      , perfisTamanho(outro.perfisTamanho)
      , gradesLength(0)
      , fo(-1) {}

Solucao& Solucao::operator=(const Solucao& outro)
{
    camada_periodo = outro.camada_periodo;
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


void Solucao::init()
{
    horario = new Horario(blocosTamanho, camadasTamanho);
    fo = -1;
    gradesLength = 0;
}

Solucao::~Solucao()
{
    delete horario;

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

