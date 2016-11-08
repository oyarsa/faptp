#include <faptp-lib/ILS.h>
#include <faptp-lib/Resolucao.h>

ILS::ILS(const Resolucao& res, int num_iter, int p_max, int p0, int max_iter,
         long long timeout)
    : res_(res),
      num_iter_(num_iter),
      p_max_(p_max),
      p0_(p0),
      max_iter_(max_iter),
      timeout_(timeout),
      tempo_fo_(),
      maior_fo_() {}

std::unique_ptr<Solucao> ILS::gerar_horario(const Solucao& s_inicial)
{
    t_.reset();
    maior_fo_ = s_inicial.getFO();
    tempo_fo_ = t_.elapsed();

    auto s_atual = descent_phase(s_inicial);
    auto s_best = s_atual->clone();

    auto p_size = p0_;
    auto iter = 0;

    for (auto i = 0; i < num_iter_ && t_.elapsed() < timeout_; i++) {
        for (auto j = 0; j < p_size && t_.elapsed() < timeout_; j++) {
            s_atual = perturbacao(*s_atual);
        }

        auto s_viz = descent_phase(*s_atual);

        if (s_viz->getFO() > s_best->getFO()) {
            s_atual = std::move(s_viz);
            s_best = s_atual->clone();

            iter = 0;
            p_size = p0_;

            maior_fo_ = s_best->getFO();
            tempo_fo_ = t_.elapsed();
        } else {
            s_atual = s_best->clone();
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

long long ILS::tempo_fo() const
{
    return tempo_fo_;
}

int ILS::maior_fo() const
{
    return maior_fo_;
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
    static const std::unordered_set<Resolucao::Vizinhanca> movimentos {
            Resolucao::Vizinhanca::ES,
            Resolucao::Vizinhanca::EM,
            Resolucao::Vizinhanca::RS,
            Resolucao::Vizinhanca::RM
    };

    auto s_best = solucao.clone();
    auto iter = 0;

    while (iter < max_iter_ && t_.elapsed() < timeout_) {
        iter++;
        auto s_viz = gerar_vizinho(escolher_vizinhanca(movimentos), *s_best);
        if (s_viz->getFO() >= s_best->getFO()) {
            iter = 0;
            s_best = move(s_viz);
        }
    }

    return s_best;
}

Resolucao::Vizinhanca ILS::escolher_vizinhanca(
    const std::unordered_set<Resolucao::Vizinhanca>& movimentos
) const
{
    return Util::randomChoice(movimentos);
}

std::unique_ptr<Solucao> ILS::perturbacao(const Solucao& solucao) const
{
    if (Util::randomDouble() < 0.5) {
        return res_.permute_resources(solucao);
    } else {
        return res_.kempe_move(solucao);
    }
}

