#include <numeric>
#include <functional>

#include <gsl/gsl>

#include <faptp-lib/Solucao.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/Aleatorio.h>

Solucao::Solucao(
    int pBlocosTamanho,
    int pCamadasTamanho,
    int pPerfisTamanho,
    const Resolucao& res,
    Configuracao::TipoFo tipo_fo
)
    : id(aleatorio::randomInt())
    , blocosTamanho(pBlocosTamanho)
    , camadasTamanho(pCamadasTamanho)
    , perfisTamanho(pPerfisTamanho)
    , horario(std::make_unique<Horario>(blocosTamanho, camadasTamanho))
    , grades()
    , gradesLength(0)
    , res(res) 
    , tipo_fo(tipo_fo) {}

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
      , res(outro.res)
      , tipo_fo(outro.tipo_fo)
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

std::unique_ptr<Solucao> Solucao::clone() const
{
    return std::make_unique<Solucao>(*this);
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
    switch (tipo_fo) {
    case Configuracao::TipoFo::Soma_carga: 
        fo = calculaFOSomaCarga();
        break;
    case Configuracao::TipoFo::Soft_constraints: 
        fo = calculaFOSoftConstraints();
        break;
    }
}

int Solucao::calculaFOSomaCarga()
{
    res.gerarGrade(this);
    return std::accumulate(begin(grades), end(grades), 0, [](int acc, auto el) {
        return acc + gsl::narrow_cast<int>(el.second->getFO());
    });
}

int Solucao::calculaFOSoftConstraints() const
{
    auto val =
        horario->contaJanelas() +
        horario->intervalosTrabalho(res.getProfessores()) +
        horario->numDiasAula() +
        horario->aulasSabado() +
        horario->aulasSeguidas(res.getDisciplinas()) +
        horario->aulasSeguidasDificil() +
        horario->aulaDificilUltimoHorario() +
        horario->preferenciasProfessores(res.getProfessores()) +
        horario->aulasProfessores(res.getProfessores());
    return -val;
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

std::unordered_map<std::string, int> Solucao::reportarViolacoes() const
{
    std::unordered_map<std::string, int> m;
    m["Janelas"] = horario->contaJanelas();
    m["Intervalos"] = horario->intervalosTrabalho(res.getProfessores());
    m["Dias de Aula"] = horario->numDiasAula();
    m["Sabado"] = horario->aulasSabado();
    m["Seguidas"] = horario->aulasSeguidas(res.getDisciplinas());
    m["Seguidas Dificil"] = horario->aulasSeguidasDificil();
    m["Dificil Ultimo Horario"] = horario->aulaDificilUltimoHorario();
    m["Preferencias disc"] = horario->preferenciasProfessores(res.getProfessores());
    m["Preferencias aulas"] = horario->aulasProfessores(res.getProfessores());
    return m;
}

const Horario& Solucao::getHorario() const
{
    return *horario;
}

Horario& Solucao::getHorario()
{
    return *horario;
}

std::size_t Solucao::getHash()
{
    return horario->getHash();
}

