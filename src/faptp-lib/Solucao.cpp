#include <numeric>
#include <functional>

#include <faptp-lib/Solucao.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/Aleatorio.h>

const std::unordered_map<std::string, double> Solucao::pesos_padrao {
  { "Janelas", 2 },
  { "IntervalosTrabalho", 1.5 },
  { "NumDiasAula", 3.5 },
  { "AulasSabado", 4.667 },
  { "AulasSeguidas", 4 },
  { "AulasSeguidasDificil", 2.5 },
  { "AulaDificilUltimoHorario", 2.333 },
  { "PreferenciasProfessores", 3.167 },
  { "AulasProfessores", 1.667 }
};

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
    , fo(std::nullopt)
    , res(res) 
    , tipo_fo(tipo_fo)
{}

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
    if (!alvoInsercao) {
        gradesLength++;
    }
    delete alvoInsercao;
    alvoInsercao = grade;
}

void Solucao::calculaFO()
{
    switch (tipo_fo) {
    case Configuracao::TipoFo::Soma_carga: 
        res.gerarGrade(this);
        fo = calculaFOSomaCarga();
        break;
    case Configuracao::TipoFo::Soft_constraints: 
        fo = calculaFOSoftConstraints();
        break;
    }
}

Solucao::FO_t
Solucao::calculaFOSomaCarga() const
{
  return std::accumulate(begin(grades), end(grades), FO_t{ 0 }, 
		[](auto acc, auto cur) {
			return acc + static_cast<int>(cur.second->getFO());
		}
	);
}

Solucao::FO_t Solucao::calculaFOSoftConstraints() const
{
  const auto& h = *horario;
  const auto& pesos = res.pesos_soft;

  auto fo_ =
    pesos.at("Janelas") * h.contaJanelas() + 
    pesos.at("IntervalosTrabalho") * h.intervalosTrabalho(res.getProfessores()) + 
    pesos.at("NumDiasAula") * h.numDiasAula() +
    pesos.at("AulasSabado") * h.aulasSabado() +
    pesos.at("AulasSeguidas") * h.aulasSeguidas(res.getDisciplinas()) +
    pesos.at("AulasSeguidasDificil") * h.aulasSeguidasDificil() +
    pesos.at("AulaDificilUltimoHorario") * h.aulaDificilUltimoHorario() +
    pesos.at("PreferenciasProfessores") * h.preferenciasProfessores(res.getProfessores()) + 
    pesos.at("AulasProfessores") * h.aulasProfessores(res.getProfessores());

  return -fo_;
}

Solucao::FO_t Solucao::getFO()
{
    if (!fo) {
        calculaFO();
    }
    return *fo;
}

Solucao::FO_t Solucao::getFO() const
{
    if (!fo) {
        throw std::runtime_error{"FO não calculada"};
    }
    return *fo;
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

