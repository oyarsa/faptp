#pragma once

#include <memory>
#include <vector>
#include <array>
#include <utility>

#include "Resolucao.h"
#include "Solucao.h"

class SA
{
public:
    SA(Resolucao& res, double alfa, double t0, int max_iter,
       int max_reheats, long long timeout,
       const std::vector<std::pair<Resolucao::Vizinhanca, int>>& chances);

    std::unique_ptr<Solucao> gerar_horario(const Solucao& s_inicial) const;

    Resolucao& res();

    double alfa() const;
    void set_alfa(double alfa);

    double t0() const;
    void set_t0(double t0);

    int max_iter() const;
    void set_max_iter(int max_iter);

    int max_reheats() const;
    void set_max_reheats(int max_reheats);

    long long timeout() const;
    void set_timeout(long long timeout);

private:
    std::unique_ptr<Solucao> gerar_vizinho(const Solucao& solucao) const;
    Resolucao::Vizinhanca escolher_vizinhanca() const;

    Resolucao& res_;
    double alfa_;
    double t0_;
    int max_iter_;
    int max_reheats_;
    long long timeout_;
    std::array<Resolucao::Vizinhanca, 100> chances_vizinhancas_;
};

inline Resolucao& SA::res()
{
    return res_;
}

inline double SA::alfa() const
{
    return alfa_;
}

inline void SA::set_alfa(double alfa)
{
    alfa_ = alfa;
}

inline double SA::t0() const
{
    return t0_;
}

inline void SA::set_t0(double t0)
{
    t0_ = t0;
}

inline int SA::max_iter() const
{
    return max_iter_;
}

inline void SA::set_max_iter(int max_iter)
{
    max_iter_ = max_iter;
}

inline int SA::max_reheats() const
{
    return max_reheats_;
}

inline void SA::set_max_reheats(int max_reheats)
{
    max_reheats_ = max_reheats;
}

inline long long SA::timeout() const
{
    return timeout_;
}

inline void SA::set_timeout(long long timeout)
{
    timeout_ = timeout;
}

