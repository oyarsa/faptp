#pragma once

#include "Resolucao.h"

class HySST
{
public:
    HySST(Resolucao& res);

    std::unique_ptr<Solucao> gerar_horario(const Solucao& s_inicial) const;

    Resolucao& res();

    long long tempo_total() const;
    void set_tempo_total(long long tempo_total);

    long long tempo_mutation() const;
    void set_tempo_mutation(long long tempo_mutation);

    long long tempo_hill() const;
    void set_tempo_hill(long long tempo_hill);

private:
    Resolucao& res_;
    long long tempo_total_;
    long long tempo_mutation_;
    long long tempo_hill_;
};

inline Resolucao& HySST::res()
{
    return res_;
}
