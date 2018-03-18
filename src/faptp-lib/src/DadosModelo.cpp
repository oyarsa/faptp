#include <faptp-lib/Disciplina.h>
#include <faptp-lib/DadosModelo.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/Professor.h>
#include <faptp-lib/Constantes.h>
#include <algorithm>
#include <iterator>

constexpr auto num_constraints = 9;

DadosModelo::DadosModelo(const Resolucao& res)
  : num_dias_{ static_cast<std::size_t>(dias_semana_util) },
    num_horarios_{ static_cast<std::size_t>(res.getBlocosTamanho()) },
    num_professores_{ res.getProfessores().size() },
    num_disciplinas_{ res.getDisciplinas().size() },
    num_camadas_{ static_cast<std::size_t>(res.getCamadasTamanho()) },
    pesos_constraints_(num_constraints),
    habilitado_(num_professores_, std::vector<char>(num_disciplinas_)),
    pertence_periodo_(num_disciplinas_, std::vector<char>(num_camadas_)),
    disponivel_(num_professores_, 
               std::vector<std::vector<char>>(num_horarios_, 
                                              std::vector<char>(num_dias_))),
    dificil_(num_disciplinas_),
    preferencia_(num_professores_, std::vector<char>(num_disciplinas_)),
    aulas_desejadas_(num_professores_),
    horas_contrato_(num_professores_),
    carga_horaria_(num_disciplinas_),
    oferecida_(num_disciplinas_, true),
    aulas_geminadas_(num_disciplinas_)
{
  auto camadas = atribuir_camadas(res.getDisciplinas());
  std::transform(begin(camadas), end(camadas), std::back_inserter(periodos_),
                 [](auto& p) { return p.first;  });

  const auto& discs = res.getDisciplinas();
  std::transform(begin(discs), end(discs), std::back_inserter(disciplinas_),
                 [](auto& d) { return d->getId(); });

  const auto& profs = res.getProfessores();
  std::transform(begin(profs), end(profs), std::back_inserter(professores_),
                 [](auto& p) { return p.second->getId(); });

  carregar_pesos(res.pesos_soft);
  carregar_habilitacoes(discs, profs);
  carregar_periodos(discs);
  carregar_disponibilidade(profs);
  carregar_dificeis(discs);
  carregar_preferencias(discs, profs);

  std::transform(begin(profs), end(profs), begin(aulas_desejadas_),
                 [](auto& p) { return p.second->preferenciaAulas(); });

  std::transform(begin(profs), end(profs), begin(horas_contrato_),
                 [](auto& p) { return p.second->credito_maximo(); });

  std::transform(begin(discs), end(discs), begin(carga_horaria_),
                 [](auto d) { return d->getCargaHoraria(); });

  // Todas as disciplinas são oferecidas. `oferecidas_` é inicializado com tudo true.

  std::transform(begin(discs), end(discs), begin(aulas_geminadas_),
                 [](auto d) { return d->getCargaHoraria() / 2; });
}

std::size_t DadosModelo::num_dias() const
{
  return num_dias_;
}

std::size_t DadosModelo::num_horarios() const
{
  return num_horarios_;
}

std::size_t DadosModelo::num_professores() const
{
  return num_professores_;
}

std::size_t DadosModelo::num_disciplinas() const
{
  return num_disciplinas_;
}

std::size_t DadosModelo::num_camadas() const
{
  return num_camadas_;
}

const std::vector<std::string>& DadosModelo::periodos() const
{
  return periodos_;
}

const std::vector<std::string>& DadosModelo::disciplinas() const
{
  return disciplinas_;
}

const std::vector<std::string>& DadosModelo::professores() const
{
  return professores_;
}

const std::vector<double>& DadosModelo::pesos_constraints() const
{
  return pesos_constraints_;
}

const std::vector<std::vector<char>>& DadosModelo::habilitado() const
{
  return habilitado_;
}

const std::vector<std::vector<char>>& DadosModelo::pertence_periodo() const
{
  return pertence_periodo_;
}

const std::vector<std::vector<std::vector<char>>>& DadosModelo::disponivel() const
{
  return disponivel_;
}

const std::vector<char>& DadosModelo::dificil() const
{
  return dificil_;
}

const std::vector<std::vector<char>>& DadosModelo::preferencia() const
{
  return preferencia_;
}

const std::vector<int>& DadosModelo::aulas_desejadas() const
{
  return aulas_desejadas_;
}

const std::vector<int>& DadosModelo::horas_contrato() const
{
  return horas_contrato_;
}

const std::vector<int>& DadosModelo::carga_horaria() const
{
  return carga_horaria_;
}

const std::vector<char>& DadosModelo::oferecida() const
{
  return oferecida_;
}

const std::vector<int>& DadosModelo::aulas_geminadas() const
{
  return aulas_geminadas_;
}

void
DadosModelo::carregar_pesos(const tsl::robin_map<std::string, double>& pesos)
{
  pesos_constraints_[0] = pesos.at("Janelas");
  pesos_constraints_[1] = pesos.at("IntervalosTrabalho");
  pesos_constraints_[2] = pesos.at("NumDiasAula");
  pesos_constraints_[3] = pesos.at("AulasSabado");
  pesos_constraints_[4] = pesos.at("AulasSeguidas");
  pesos_constraints_[5] = pesos.at("AulasSeguidasDificil");
  pesos_constraints_[6] = pesos.at("AulaDificilUltimoHorario");
  pesos_constraints_[7] = pesos.at("PreferenciasProfessores");
  pesos_constraints_[8] = pesos.at("AulasProfessores");
}

void
DadosModelo::carregar_habilitacoes(
  const std::vector<Disciplina*>& disciplinas, 
  const tsl::robin_map<std::string, Professor*>& professores)
{
  auto i = 0;
  auto j = 0;

  for (auto& par : professores) {
    auto p = par.second;
    for (auto d : disciplinas) {
      habilitado_[i][j] = p->haveCompetencia(d->getId());
      j++;
    }
    i++;
  }
}

void
DadosModelo::carregar_periodos(const std::vector<Disciplina*>& disciplinas)
{
  auto camadas = atribuir_camadas(disciplinas);

  for (auto i = 0u; i < disciplinas.size(); i++) {
    for (auto j = 0u; i < camadas.size(); j++) {
      pertence_periodo_[i][j] = j == camadas[disciplinas[i]->getPeriodo()];
    }
  }
}

tsl::robin_map<std::string, std::size_t>
DadosModelo::atribuir_camadas(const std::vector<Disciplina*>& disciplinas)
{
  tsl::robin_map<std::string, std::size_t> m;
  auto next = 0;

  for (auto d : disciplinas) {
    if (!m[d->getPeriodo()]) m[d->getPeriodo()] = next++;
  }

  return m;
}

void
DadosModelo::carregar_disponibilidade(
  const tsl::robin_map<std::string, Professor*>& professores)
{
  auto p = 0;
  for (auto& par : professores) {
    for (auto i = 0u; i < num_horarios_; i++) {
      for (auto j = 0u; j < num_dias_; j++) {
        disponivel_[p][i][j] = par.second->isDiaDisponivel(j, i);
      }
    }
    p++;
  }
}

void
DadosModelo::carregar_dificeis(const std::vector<Disciplina*>& disciplinas)
{
  for (auto i = 0u; i < disciplinas.size(); i++) {
    dificil_[i] = disciplinas[i]->isDificil();
  }
}

void
DadosModelo::carregar_preferencias(const std::vector<Disciplina*>& disciplinas, const tsl::robin_map<std::string, Professor*>& professores)
{
  auto i = 0;
  auto j = 0;

  for (auto& p : professores) {
    for (auto d : disciplinas) {
      preferencia_[i][j] = p.second->isDiscPreferencia(d->id_hash());
      j++;
    }
    i++;
  }
}

