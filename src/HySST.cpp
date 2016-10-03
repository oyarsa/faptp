#include "HySST.h"

#include "Resolucao.h"
#include "Timer.h"

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
    auto t_total = Timer();
    auto s_atual = std::make_unique<Solucao>(s_inicial);
    auto s_best = std::make_unique<Solucao>(s_inicial);
    auto level = 0;

    while (t_total.elapsed() < tempo_total_) {
        auto s_stage_best = std::make_unique<Solucao>(*s_atual);
        auto s_stage_start = std::make_unique<Solucao>(*s_atual);
        auto eps = thresholds_[level];

        auto t_mu = Timer();
        while (t_mu.elapsed() < tempo_mutation_ && t_total.elapsed() < tempo_total_) {
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

        auto t_hc = Timer();
        if (s_stage_best->getFO() <= s_stage_start->getFO()) {
            while (t_hc.elapsed() < tempo_hill_ && t_total.elapsed() < tempo_total_) {
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
    auto s = solucao.clone();
    auto slot = pick_place(*s);

    for (auto i = 0; i < it_hc_; i++) {
        auto cur = s->clone();
        if (auto next = ejection_move(*cur, slot)) {
            slot = *next;
            s = move(cur);
        } else {
            slot = pick_place(*s);
        }
    }

    return s;
}

ex::optional<HySST::Time_slot> HySST::ejection_move(
    Solucao& solucao, 
    Time_slot place
) const
{
    auto slot = pick_event_and_move(solucao, place);
    if (!slot) {
        return ex::nullopt;
    }

    auto liebhabers = list_all_liebhabers(solucao, *slot);
    return choose_and_move(solucao, liebhabers, *slot);
}

HySST::Time_slot HySST::pick_place(const Solucao& solucao) const
{
    const auto& horario = solucao.getHorario();

    auto num_slots_percorridos = 0u;
    auto num_slots = horario.getMatriz().size() / 2;
    std::vector<bool> slots_percorridos(num_slots, false);

    while (num_slots_percorridos < num_slots) {
        auto slot = [&] {
            int s;
            do {
                s = Util::randomBetween(0, num_slots);
            } while (slots_percorridos[s]);
            return s;
        }();
        num_slots_percorridos++;
        slots_percorridos[slot] = true;

        if (!horario.at(2*slot) && !horario.at(2*slot+1)) {
            return horario.getCoords(2*slot);
        }
    }

    return {0, 0, 0};
}

ex::optional<HySST::Time_slot> HySST::pick_event_and_move(
    Solucao& solucao, 
    Time_slot slot
) const
{
    auto& horario = solucao.getHorario();

    int dia, bloco, camada;
    std::tie(dia, bloco, camada) = slot;

    auto novo_dia = Util::randomBetween(0, dias_semana_util);
    auto novo_bloco = Util::randomBetween(0, res_.getBlocosTamanho());

    auto pd1 = horario.at(novo_dia, novo_bloco, camada);
    auto pd2 = horario.at(novo_dia, novo_bloco+1, camada);
    horario.clearSlot(novo_dia, novo_bloco, camada);
    horario.clearSlot(novo_dia, novo_bloco+1, camada);

    if (horario.insert(dia, bloco, camada, pd1) 
        && horario.insert(dia, bloco+1, camada, pd2)) {
        return std::make_tuple(novo_dia, novo_bloco, camada);
    } else {
        return ex::nullopt;
    }
}

std::vector<HySST::Event> HySST::list_all_liebhabers(
    const Solucao& solucao, 
    Time_slot slot
) const
{
    int dia, bloco, camada;
    std::tie(dia, bloco, camada) = slot;

    std::vector<Event> liebhabers;

    auto periodo = solucao.camada_periodo.at(camada);
    auto disciplinas = res_.getPeriodoXDisciplinas().at(periodo);
    auto horario = solucao.getHorario();

    for (auto d = 0; d < dias_semana_util; d++) {
        for (auto b = 0; b < horario.getBlocosTamanho(); b++) {
            auto pd = horario.at(d, b, camada);
            horario.clearSlot(d, b, camada);
            if (horario.isViable(dia, bloco, camada, pd)) {
                liebhabers.push_back({{d, b, camada}, pd});
            }
            horario.insert(d, b, camada, pd);
        }
    }

    return liebhabers;
}

ex::optional<HySST::Time_slot> HySST::choose_and_move(
    Solucao& solucao, 
    const std::vector<Event>& liebhabers,
    Time_slot dest
) const
{
    ProfessorDisciplina* pd;
    int d_og, b_og, c_og;
    Time_slot slot;
    std::tie(slot, pd) = Util::randomChoice(liebhabers);
    std::tie(b_og, d_og, c_og) = slot;

    int d_dest, b_dest, c_dest;
    std::tie(d_dest, b_dest, c_dest) = dest;

    auto& horario = solucao.getHorario();
    horario.clearSlot(d_og, b_og, c_og);
    if (horario.insert(d_dest, b_dest, c_dest, pd)) {
        return std::make_tuple(d_og, b_og, c_og);
    } else {
        return ex::nullopt;
    }
}
