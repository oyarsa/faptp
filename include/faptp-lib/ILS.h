#pragma once
#include <memory>
#include <set>

#include "Solucao.h"
#include "Resolucao.h"
#include "Timer.h"

class ILS
{
public:
    ILS(const Resolucao& res, int num_iter, int p_max, int p0, int max_iter,
        long long timeout);

    std::unique_ptr<Solucao> gerar_horario(const Solucao& s_inicial);

    int num_iter() const;
    void set_num_iter(int num_iter);

    int p_max() const;
    void set_p_max(int p_max);

    int p0() const;
    void set_p0(int p0);

    int max_iter() const;
    void set_max_iter(int max_iter);

    long long timeout() const;
    void set_timeout(long long timeout);

    long long tempo_fo() const;
    int maior_fo() const;

private:
    std::unique_ptr<Solucao> gerar_vizinho(Resolucao::Vizinhanca vizinhanca,
                                           const Solucao& solucao) const;
    std::unique_ptr<Solucao> descent_phase(const Solucao& solucao) const;

    Resolucao::Vizinhanca escolher_vizinhanca(
        const std::set<Resolucao::Vizinhanca>& movimentos) const;
    std::unique_ptr<Solucao> perturbacao(const Solucao& solucao) const;

    const Resolucao& res_;
    int num_iter_;
    int p_max_;
    int p0_;
    int max_iter_;
    long long timeout_;
    Timer t_;
    long long tempo_fo_;
    int maior_fo_;
};


inline int ILS::num_iter() const
{
    return num_iter_;
}

inline void ILS::set_num_iter(int num_iter)
{
    num_iter_ = num_iter;
}

inline int ILS::p_max() const
{
    return p_max_;
}

inline void ILS::set_p_max(int p_max)
{
    p_max_ = p_max;
}

inline int ILS::p0() const
{
    return p0_;
}

inline void ILS::set_p0(int p0)
{
    p0_ = p0;
}

inline int ILS::max_iter() const
{
    return max_iter_;
}

inline void ILS::set_max_iter(int max_iter)
{
    max_iter_ = max_iter;
}

inline long long ILS::timeout() const
{
    return timeout_;
}

inline void ILS::set_timeout(long long timeout)
{
    timeout_ = timeout;
}


