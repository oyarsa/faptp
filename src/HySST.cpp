#include "HySST.h"

HySST::HySST(Resolucao& res) 
    : res_(res) {}

std::unique_ptr<Solucao> HySST::gerar_horario(const Solucao& s_inicial) const
{
    return{};
}

long long HySST::tempo_total() const
{
    return tempo_total_;
}

void HySST::set_tempo_total(long long tempo_total)
{
    tempo_total_ = tempo_total;
}

long long HySST::tempo_mutation() const
{
    return tempo_mutation_;
}

void HySST::set_tempo_mutation(long long tempo_mutation)
{
    tempo_mutation_ = tempo_mutation;
}

long long HySST::tempo_hill() const
{
    return tempo_hill_;
}

void HySST::set_tempo_hill(long long tempo_hill)
{
    tempo_hill_ = tempo_hill;
}
