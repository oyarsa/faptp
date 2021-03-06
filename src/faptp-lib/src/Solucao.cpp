#include <numeric>
#include <functional>

#include <faptp-lib/Solucao.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/Aleatorio.h>

const tsl::robin_map<std::string, double> Solucao::pesos_padrao {
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

  auto janelas = pesos.at("Janelas");
  auto intervalos = pesos.at("IntervalosTrabalho");
  auto dias_aula = pesos.at("NumDiasAula");
  auto aulas_sabdo = pesos.at("AulasSabado");
  auto aulas_seguidas = pesos.at("AulasSeguidas");
  auto aulas_seguidas_dif = pesos.at("AulasSeguidasDificil");
  auto aula_dificil = pesos.at("AulaDificilUltimoHorario");
  auto pref = pesos.at("PreferenciasProfessores");
  auto aulas_prof = pesos.at("AulasProfessores");

  auto fo1 = janelas * h.contaJanelas();
  auto fo2 = intervalos * h.intervalosTrabalho();
  auto fo3 = dias_aula * h.numDiasAula();
  auto fo4 = aulas_sabdo * h.aulasSabado();
  auto fo5 = aulas_seguidas * h.aulasSeguidas();
  auto fo6 = aulas_seguidas_dif * h.aulasSeguidasDificil();
  auto fo7 = aula_dificil * h.aulaDificilUltimoHorario();
  auto fo8 = pref * h.preferenciasProfessores();
  auto fo9 = aulas_prof * h.aulasProfessores(res.getProfessores());

  auto fo_ = fo1 + fo2 + fo3 + fo4 + fo5 + fo6 + fo7 + fo8 + fo9;

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
        throw std::runtime_error{"FO nao calculada"};
    }
    return *fo;
}

bool Solucao::hasFO() const
{
  return fo.has_value();
}

tsl::robin_map<std::string, int> Solucao::reportarViolacoes() const
{
    tsl::robin_map<std::string, int> m;

    m["Janelas"] = horario->contaJanelas();
    m["IntervalosTrabalho"] = horario->intervalosTrabalho();
    m["NumDiasAula"] = horario->numDiasAula();
    m["AulasSabado"] = horario->aulasSabado();
    m["AulasSeguidas"] = horario->aulasSeguidas();
    m["AulasSeguidasDificil"] = horario->aulasSeguidasDificil();
    m["AulaDificilUltimoHorario"] = horario->aulaDificilUltimoHorario();
    m["PreferenciasProfessores"] = horario->preferenciasProfessores();
    m["AulasProfessores"] = horario->aulasProfessores(res.getProfessores());

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

