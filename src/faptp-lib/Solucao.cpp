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
	Configuracao::TipoFo tipo_fo,
	const boost::optional<std::array<double, 9>>& pesos
)
    : id(aleatorio::randomInt())
    , blocosTamanho(pBlocosTamanho)
    , camadasTamanho(pCamadasTamanho)
    , perfisTamanho(pPerfisTamanho)
    , horario(std::make_unique<Horario>(blocosTamanho, camadasTamanho))
    , grades()
    , gradesLength(0)
    , res(res) 
    , tipo_fo(tipo_fo)
{
	if (pesos) {
		pesos_ = *pesos;
	} else {
		pesos_ = k_pesos_padrao;
	}
}

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
      , pesos_(outro.pesos_)
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

Solucao::FO_t Solucao::calculaFOSomaCarga()
{
    res.gerarGrade(this);
	return std::accumulate(begin(grades), end(grades), Solucao::FO_t{ 0 }, 
		[](auto acc, auto el) {
			return acc + el.second->getFO();
		}
	);
}

Solucao::FO_t Solucao::calculaFOSoftConstraints() const
{
	std::array<int, k_num_pesos> penalidades = { {
		horario->contaJanelas(),
		horario->intervalosTrabalho(res.getProfessores()),
		horario->numDiasAula(),
		horario->aulasSabado(),
		horario->aulasSeguidas(res.getDisciplinas()),
		horario->aulasSeguidasDificil(),
		horario->aulaDificilUltimoHorario(),
		horario->preferenciasProfessores(res.getProfessores()),
		horario->aulasProfessores(res.getProfessores())
	} };

	auto fo = std::inner_product(begin(pesos_), end(pesos_), begin(penalidades), 0.0);
	return fo;
}

Solucao::FO_t Solucao::getFO()
{
    if (fo == -1) {
        calculaFO();
    }
    return fo;
}

Solucao::FO_t Solucao::getFO() const
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

