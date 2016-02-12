#ifndef SORTTEMPLATE_H
#define SORTTEMPLATE_H

#include <functional>
#include <string>

#include "../src/ProfessorDisciplina.h"

template<typename T, typename M, template<typename> class C = std::less>
struct member_comparer : std::binary_function<T, T, bool> {

    explicit member_comparer(M T::*p) : p_(p) {
    }

    bool operator()(T const& lhs, T const& rhs) const {
        return C<M>()(lhs.*p_, rhs.*p_);
    }

private:
    M T::*p_;
};

template<typename T, typename M>
member_comparer<T, M> make_member_comparer(M T::*p) {
    return member_comparer<T, M>(p);
}

template<template<typename> class C, typename T, typename M>
member_comparer<T, M, C> make_member_comparer2(M T::*p) {
    return member_comparer<T, M, C>(p);
}

template<typename T, typename M, template<typename> class C = std::less>
struct method_comparer : std::binary_function<T, T, bool> {

    explicit method_comparer(M(T::*p)() const) : p_(p) {
    }

    bool operator()(T const& lhs, T const& rhs) const {
        return C<M>()((lhs.*p_)(), (rhs.*p_)());
    }

private:
    M(T::*p_)() const;
};

template<typename T, typename M>
method_comparer<T, M> make_method_comparer(M(T::*p)() const) {
    return method_comparer<T, M>(p);
}

template<template<typename> class C, typename T, typename M>
method_comparer<T, M, C> make_method_comparer2(M(T::*p)() const) {
    return method_comparer<T, M, C>(p);
}

struct DisciplinaCargaHorariaDesc {

    bool operator()(const Disciplina *a, const Disciplina *b) {
        return a->getCargaHoraria() > b->getCargaHoraria();
    }
};

struct ProfessorDisciplinaCargaHorariaDesc {

    bool operator()(const ProfessorDisciplina *a, const ProfessorDisciplina *b) {
        return a->getDisciplina()->getCargaHoraria() > b->getDisciplina()->getCargaHoraria();
    }
};

struct HorarioFindDisciplina {
    const Disciplina* disciplina;

    HorarioFindDisciplina(const Disciplina* pDisciplina) : disciplina(pDisciplina) {
    }

    bool operator()(const ProfessorDisciplina* pProfessorDisciplina) const {
        bool find = false;
        if (pProfessorDisciplina != NULL) {
            find = pProfessorDisciplina->getDisciplina() == disciplina;
        } else if (disciplina == NULL) {
            find = (pProfessorDisciplina == NULL);
        }
        return find;
    }
};

struct DisciplinaFindDisciplina {
    Disciplina* disciplina;

    DisciplinaFindDisciplina(Disciplina* pDisciplina) : disciplina(pDisciplina) {
    }

    bool operator()(const Disciplina* d) {
        return disciplina == d;
    }
};

struct DisciplinasRemoveDisciplinas {
    std::vector<Disciplina*> disciplinas;

    DisciplinasRemoveDisciplinas(std::vector<Disciplina*> pDisciplinas) : disciplinas(pDisciplinas) {
    }

    bool operator()(Disciplina* d) {
        return find_if(disciplinas.begin(), disciplinas.end(), DisciplinaFindDisciplina(d)) != disciplinas.end();
    }
};

struct DisciplinaFindDisciplinaId {

    DisciplinaFindDisciplinaId(std::string key) : key_(key) {
    }

    bool operator()(const Disciplina* d) const {
        return key_ == d->getId();
    }

    const std::string key_;
};

#endif /* SORTTEMPLATE_H */

