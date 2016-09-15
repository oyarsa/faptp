#include "HySST.h"

#include "Resolucao.h"

HySST::HySST(const Resolucao& res, long long tempo_total, long long tempo_mutation,
             long long tempo_hill, int max_level, int t_start, int t_step,
             int it_hc)
    : res_(res),
      tempo_total_(tempo_total),
      tempo_mutation_(tempo_mutation),
      tempo_hill_(tempo_hill),
      max_level_(max_level),
      thresholds_(gen_thresholds(max_level, t_start, t_step)), 
      it_hc_(it_hc) {}

std::unique_ptr<Solucao> HySST::gerar_horario(const Solucao& s_inicial) const
{
    auto t_inicio = Util::now();
    auto s_atual = std::make_unique<Solucao>(s_inicial);
    auto s_best = std::make_unique<Solucao>(s_inicial);
    auto level = 0;

    while (Util::chronoDiff(Util::now(), t_inicio) < tempo_total_) {
        auto s_stage_best = std::make_unique<Solucao>(*s_atual);
        auto s_stage_start = std::make_unique<Solucao>(*s_atual);
        auto eps = thresholds_[level];

        auto t_mu_inicio = Util::now();
        while (Util::chronoDiff(Util::now(), t_mu_inicio) < tempo_mutation_
               && Util::chronoDiff(Util::now(), t_inicio) < tempo_total_) {
            auto llh = Util::randomChoice(heuristicas_mutacionais_);
            auto s_viz = aplicar_heuristica(llh, *s_atual);

            if (s_viz->getFO() > s_stage_best->getFO()) {
                s_best = std::make_unique<Solucao>(*s_viz);
            }
            if (s_viz->getFO() + eps > s_best->getFO()) {
                s_stage_best = std::make_unique<Solucao>(*s_viz);
            }
            if (s_viz->getFO() + eps > s_atual->getFO()) {
                s_atual = std::make_unique<Solucao>(*s_viz);
            }
        }

        auto t_hc_inicio = Util::now();
        if (s_stage_best->getFO() <= s_stage_start->getFO()) {
            while (Util::chronoDiff(Util::now(), t_hc_inicio) < tempo_hill_
                   && Util::chronoDiff(Util::now(), t_inicio) < tempo_total_) {
                auto llh = Util::randomChoice(heuristicas_hill_);
                auto s_viz = aplicar_heuristica(llh, *s_atual);

                if (s_viz->getFO() > s_stage_best->getFO()) {
                    s_best = std::make_unique<Solucao>(*s_viz);
                }
                if (s_viz->getFO() > s_best->getFO()) {
                    s_stage_best = std::make_unique<Solucao>(*s_viz);
                }
                s_atual = std::move(s_viz);
            }
        }

        if (s_stage_best->getFO() <= s_stage_start->getFO()) {
            if (level == max_level_ - 1) {
                s_atual = std::move(s_stage_start);
                level = 0;
            } else {
                level++;
            }
        }
    }

    return s_best;
}

const Resolucao& HySST::res() const
{
    return res_;
}

long long HySST::tempo_total() const
{
    return tempo_total_;
}

long long HySST::tempo_mutation() const
{
    return tempo_mutation_;
}

long long HySST::tempo_hill() const
{
    return tempo_hill_;
}

int HySST::max_level() const
{
    return max_level_;
}

std::vector<int> HySST::gen_thresholds(int max_level, int t_start, int t_step)
{
    std::vector<int> thresholds(max_level);
    auto curr_t = t_start;

    for (auto i = 0; i < max_level; i++) {
        thresholds[i] = curr_t;
        curr_t += t_step;
    }

    return thresholds;
}

std::unique_ptr<Solucao> HySST::aplicar_heuristica(Resolucao::Vizinhanca llh, 
                                                   const Solucao& solucao) const
{
    switch (llh) {
        case Resolucao::Vizinhanca::ES: return res_.event_swap(solucao);
        case Resolucao::Vizinhanca::EM: return res_.event_move(solucao);
        case Resolucao::Vizinhanca::RS: return res_.resource_swap(solucao);
        case Resolucao::Vizinhanca::RM: return res_.resource_move(solucao);
        case Resolucao::Vizinhanca::HC_FI: return first_improvement(solucao);
        case Resolucao::Vizinhanca::HC_EC: return ejection_chains(solucao);
        default: return std::make_unique<Solucao>(solucao);
    }
}

std::unique_ptr<Solucao> HySST::first_improvement(const Solucao& solucao) const
{
    auto s = std::make_unique<Solucao>(solucao);

    for (auto it = 0; it < it_hc_;) {
        auto s_viz = [&] {
            if (Util::randomDouble() < 0.5) {
                return res_.permute_resources(*s);
            } else {
                return res_.kempe_move(*s);
            }
        }();

        if (s_viz->getFO() > s->getFO()) {
            s = std::move(s_viz);
        }
    }

    return s;
}

std::unique_ptr<Solucao> HySST::ejection_chains(const Solucao& solucao) const
{
    return nullptr;
}
