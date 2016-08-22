#include "ILS.h"
#include "Resolucao.h"

ILS::ILS(Resolucao& res, int num_iter, int p_max, int p0, int max_iter,
         long long timeout)
    : res_(res),
      num_iter_(num_iter),
      p_max_(p_max),
      p0_(p0),
      max_iter_(max_iter),
      timeout_(timeout) {}

std::unique_ptr<Solucao> ILS::gerar_horario(const Solucao& s_inicial) const
{
    auto s_atual = descent_phase(s_inicial);
    auto s_best = std::make_unique<Solucao>(*s_atual);

    auto p_size = p0_;
    auto iter = 0;
    auto tempoInicial = Util::now();

    for (auto i = 0;
         i < num_iter_ && Util::chronoDiff(Util::now(), tempoInicial) < timeout_;
         i++) {

        for (auto j = 0; j < p_size; j++) {
            s_atual = perturbacao(*s_atual);
        }

        auto s_viz = descent_phase(*s_atual);

        if (s_viz->getFO() > s_best->getFO()) {
            s_atual = std::move(s_viz);
            s_best = std::make_unique<Solucao>(*s_atual);

            iter = 0;
            p_size = p0_;
        } else {
            s_atual = std::make_unique<Solucao>(*s_best);
            iter++;
        }

        if (iter == max_iter_) {
            p_size += p0_;
            if (p_size >= p_max_) {
                p_size = p0_;
            }
        }
    }

    return s_best;
}

std::unique_ptr<Solucao> ILS::gerar_vizinho(
    Resolucao::Vizinhanca vizinhanca,
    const Solucao& solucao
) const
{
    switch (vizinhanca) {
        case Resolucao::Vizinhanca::ES: return res_.event_swap(solucao);
        case Resolucao::Vizinhanca::EM: return res_.event_move(solucao);
        case Resolucao::Vizinhanca::RS: return res_.resource_swap(solucao);
        case Resolucao::Vizinhanca::RM: return res_.resource_move(solucao);
        case Resolucao::Vizinhanca::KM: return res_.kempe_move(solucao);
        default: return nullptr;
    }
}

std::unique_ptr<Solucao> ILS::descent_phase(const Solucao& solucao) const
{
    const std::unordered_set<Resolucao::Vizinhanca> movimentos {
            Resolucao::Vizinhanca::ES,
            Resolucao::Vizinhanca::EM,
            Resolucao::Vizinhanca::RS,
            Resolucao::Vizinhanca::RM,
            Resolucao::Vizinhanca::KM
    };

    auto best = std::make_unique<Solucao>(solucao);
    auto movimentos_restantes = movimentos;

    while (!movimentos_restantes.empty()) {
        auto vizinhanca = escolher_vizinhanca(movimentos_restantes);
        movimentos_restantes.erase(vizinhanca);
        auto vizinho = gerar_vizinho(vizinhanca, *best);

        if (vizinho->getFO() >= best->getFO()) {
            best = std::move(vizinho);
            movimentos_restantes = movimentos;
        }
    }

    return best;
}

Resolucao::Vizinhanca ILS::escolher_vizinhanca(
    const std::unordered_set<Resolucao::Vizinhanca>& movimentos
) const
{
    return Util::randomChoice(movimentos);
}

std::unique_ptr<Solucao> ILS::perturbacao(const Solucao& solucao) const
{
    if (Util::randomDouble() <= 0.5) {
        return res_.permute_resources(solucao);
    } else {
        return res_.kempe_move(solucao);
    }
}

Resolucao& ILS::res()
{
    return res_;
}
