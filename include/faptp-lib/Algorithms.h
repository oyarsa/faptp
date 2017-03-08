#ifndef SORTTEMPLATE_H
#define SORTTEMPLATE_H

#include <string>
#include <unordered_set>
#include <faptp-lib/Disciplina.h>
#include <faptp-lib/ProfessorDisciplina.h>
#include <faptp-lib/Solucao.h>

struct DisciplinaCargaHorariaDesc
{
    bool operator()(const Disciplina* a, const Disciplina* b)
    {
        return a->getCargaHoraria() > b->getCargaHoraria();
    }
};

struct ProfessorDisciplinaCargaHorariaDesc
{
    bool operator()(const ProfessorDisciplina* a, const ProfessorDisciplina* b)
    {
        return a->getDisciplina()->getCargaHoraria() > b->getDisciplina()->getCargaHoraria();
    }
};

struct HorarioFindDisciplina
{
    const Disciplina* disciplina;

    HorarioFindDisciplina(const Disciplina* pDisciplina) : disciplina(pDisciplina) { }

    bool operator()(const ProfessorDisciplina* pProfessorDisciplina) const
    {
        bool find = false;
        if (pProfessorDisciplina != NULL) {
            find = pProfessorDisciplina->getDisciplina() == disciplina;
        } else if (disciplina == NULL) {
            find = (pProfessorDisciplina == NULL);
        }
        return find;
    }
};

struct DisciplinaFindDisciplina
{
    Disciplina* disciplina;

    DisciplinaFindDisciplina(Disciplina* pDisciplina) : disciplina(pDisciplina) { }

    bool operator()(const Disciplina* d)
    {
        return disciplina == d;
    }
};

struct DisciplinasRemoveDisciplinas
{
    std::vector<Disciplina*> disciplinas;

    DisciplinasRemoveDisciplinas(std::vector<Disciplina*> pDisciplinas) : disciplinas(pDisciplinas) { }

    bool operator()(Disciplina* d)
    {
        return find_if(disciplinas.begin(), disciplinas.end(), DisciplinaFindDisciplina(d)) != disciplinas.end();
    }
};

template <typename Container>
void RemoveDisciplinasNome(Container& c,
                           std::vector<Disciplina*>& disciplinas)
{
  for (auto it = std::begin(c); it != std::end(c); ++it) {
    const auto found = std::find_if(
      std::begin(disciplinas), std::end(disciplinas),
      [&](const Disciplina* d) {
        return d->getNome() == *it;
      });
    if (found != std::end(disciplinas)) {
      c.erase(it);
    }
  }
}

struct DisciplinaFindDisciplinaId
{
    DisciplinaFindDisciplinaId(const std::string& key) : key_(key) {}

    bool operator()(const Disciplina* d) const
    {
        return key_ == d->getId();
    }

    std::string key_;
};

struct SolucaoComparaMaior
{
    bool operator()(Solucao* lhs, Solucao* rhs) const
    {
        return lhs->getFO() > rhs->getFO();
    }
};

namespace std
{

template<>
struct less<unique_ptr<Solucao>>
{
    bool operator()(const unique_ptr<Solucao>& lhs, const unique_ptr<Solucao>& rhs) const
    {
        if (!lhs) {
            return true;
        }
        if (!rhs) {
            return false;
        }
        return lhs->getFO() < rhs->getFO();
    }
};

template<>
struct less<Solucao*>
{
    bool operator()(const Solucao* lhs, const Solucao* rhs) const
    {
        if (!lhs) {
            return true;
        }
        if (!rhs) {
            return false;
        }
        return lhs->getFO() < rhs->getFO();
    }
};

}

inline bool pd_equals(const ProfessorDisciplina* lhs,
                      const ProfessorDisciplina* rhs)
{
  if (lhs == rhs) {
    return true;
  }
  if (!lhs || !rhs) {
    return false;
  }
  return lhs->getDisciplina() == rhs->getDisciplina();
}

inline auto
procura_gene(const std::vector<ProfessorDisciplina*>& genes,
             const ProfessorDisciplina* pd)
{
  return std::find_if(begin(genes), end(genes),
                      [pd](const ProfessorDisciplina* o) {
                        return pd_equals(o, pd);
                      });
}

inline bool genes_contem(const std::vector<ProfessorDisciplina*>& genes,
                         const ProfessorDisciplina* pd)
{
  return std::find_if(begin(genes), end(genes),
                      [pd](const ProfessorDisciplina* o) {
                        return pd_equals(o, pd);
                      }) != end(genes);
}

#endif /* SORTTEMPLATE_H */

