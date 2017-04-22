#pragma once

#include <memory>
#include <faptp-lib/Solucao.h>

class Resolucao;

class HySST
{
public:
    HySST(Resolucao& res, long long tempo_total, 
          long long tempo_mutation, long long tempo_hill, 
          int max_level, int t_start, int t_step, int it_hc);

    HySST(const HySST& outro);
    HySST& operator=(const HySST& rhs);

    HySST(HySST&& outro) noexcept;
    HySST& operator=(HySST&& rhs) noexcept;

    ~HySST();

    std::unique_ptr<Solucao> gerar_horario(const Solucao& s_inicial);

    long long tempo_fo() const;
    Solucao::FO_t maior_fo() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d_;
};
