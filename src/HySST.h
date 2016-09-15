#pragma once

#include "Resolucao.h"
#include <algorithm>

class HySST
{
public:
    HySST(const Resolucao& res, long long tempo_total, 
                    long long tempo_mutation, long long tempo_hill, 
                    int max_level, int t_start, int t_step);

    std::unique_ptr<Solucao> gerar_horario(const Solucao& s_inicial) const;

    const Resolucao& res() const;
    long long tempo_total() const;
    long long tempo_mutation() const;
    long long tempo_hill() const;
    int max_level() const;

private:
    static std::vector<int> gen_thresholds(int max_level, int t_start, int t_step);
    std::unique_ptr<Solucao> aplicar_heuristica(Resolucao::Vizinhanca llh, 
                                                const Solucao& solucao) const;

    const Resolucao&       res_;
    const long long        tempo_total_;
    const long long        tempo_mutation_;
    const long long        tempo_hill_;
    const int              max_level_;
    const std::vector<int> thresholds_;

    const std::array<Resolucao::Vizinhanca, 6> heuristicas_mutacionais_{
        Resolucao::Vizinhanca::ES,
        Resolucao::Vizinhanca::EM,
        Resolucao::Vizinhanca::RS,
        Resolucao::Vizinhanca::RM,
        Resolucao::Vizinhanca::PR,
        Resolucao::Vizinhanca::RM
    };

    const std::vector<Resolucao::Vizinhanca> heuristicas_hill_{

    };
};
