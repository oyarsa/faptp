#pragma once

#include "Resolucao.h"
#include <algorithm>

class HySST
{
public:
    HySST(const Resolucao& res, long long tempo_total, long long tempo_mutation,
          long long tempo_hill, int max_level, int t_start, int t_step);

    std::unique_ptr<Solucao> gerar_horario(const Solucao& s_inicial) const;

    const Resolucao& res() const;

    long long tempo_total() const;
    void set_tempo_total(long long tempo_total);

    long long tempo_mutation() const;
    void set_tempo_mutation(long long tempo_mutation);

    long long tempo_hill() const;
    void set_tempo_hill(long long tempo_hill);

    int max_level() const;
    void set_max_level(int max_level);

private:
    static std::vector<int> gen_thresholds(int max_level, int t_start, int t_step);
    std::unique_ptr<Solucao> aplicar_heuristica(Resolucao::Vizinhanca llh, 
                                                const Solucao& solucao) const;

    const Resolucao& res_;
    long long tempo_total_;
    long long tempo_mutation_;
    long long tempo_hill_;
    int max_level_;
    std::vector<int> thresholds_;
    std::vector<Resolucao::Vizinhanca> heuristicas_mutacionais_;
    std::vector<Resolucao::Vizinhanca> heuristicas_hill_;
};
