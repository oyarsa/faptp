#pragma once

#include <memory>

#include "Solucao.h"
#include "Resolucao.h"

class WDJU
{
public:
    WDJU(const Resolucao& res, long long timeout, int stagnation_limit,
         double jump_factor);

    std::unique_ptr<Solucao> gerar_horario(const Solucao& solucao);

    long long tempo_fo() const;
    int maior_fo() const;

private:
    double next_jump(const std::vector<double>& history) const;
    std::unique_ptr<Solucao> gerar_vizinho(const Solucao& solucao) const;

    const Resolucao& res_;
    long long timeout_;
    int stagnation_limit_;
    double max_jump_factor_;
    long long tempo_fo_;
    int maior_fo_;
};
