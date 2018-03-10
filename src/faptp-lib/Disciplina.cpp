#include <algorithm>

#include <faptp-lib/Disciplina.h>
#include <faptp-lib/Util.h>

Disciplina::Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo, std::string pCurso, std::string pTurma, int pCapacidade, std::string pPeriodoMinimo)
    : Disciplina(pNome, pCargaHoraria, pPeriodo, pCurso, UUID::GenerateUuid(),
                 pTurma, pCapacidade, pPeriodoMinimo) {}

Disciplina::Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade, std::string pPeriodoMinimo)
    : id(pId),
      turma(pTurma),
      nome(pNome),
      periodo(pPeriodo),
      periodoMinimo(pPeriodoMinimo),
      curso(pCurso),
      cargaHoraria(pCargaHoraria),
      aulasSemana(cargaHoraria),
      capacidade(pCapacidade),
      alocados(0),
      ofertada(true),
      preRequisitos(),
      coRequisitos(),
      equivalentes(),
      professoresCapacitados(),
      dificil(false),
      idHash(std::hash<std::string>{}(pId))
{}

void
Disciplina::addProfessorCapacitado(Professor* professor)
{
  Util::insert_sorted(
    professoresCapacitados, professor,
    [](auto a, auto b) {
      return a->getNumDisponibilidade() < b->getNumDisponibilidade();
    });
}

bool
Disciplina::isDificil() const
{
  return dificil;
}

int Disciplina::periodoNum() const
{
    if (periodo.empty())
        return 0;

    static auto pos = periodo.find('-');
    static auto per_num = std::stoi(periodo.substr(0, pos));

    return per_num;
}

int Disciplina::periodoMinimoNum() const
{
    if (periodoMinimo.empty())
        return 0;

    static auto pos = periodoMinimo.find('-');
    static auto permin_num = std::stoi(periodoMinimo.substr(0, pos));

    return permin_num;
}

