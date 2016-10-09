#include <faptp/HySST.h>

#include <boost/optional.hpp>
#include <array>

#include <faptp/Resolucao.h>
#include <faptp/Solucao.h>
#include <faptp/Timer.h>

// Implementação

struct HySST::Impl
{
    Impl(Resolucao& res, long long tempo_total,
         long long tempo_mutation, long long tempo_hill,
         int max_level, int t_start, int t_step, int it_hc);

    using Time_slot = std::tuple<int, int, int>;
    using Event = std::pair<Time_slot, ProfessorDisciplina*>;

    static std::vector<int> gen_thresholds(int max_level, int t_start, int t_step);

    Resolucao::Vizinhanca choose_hill() const;

    Resolucao::Vizinhanca choose_mut() const;

    std::unique_ptr<Solucao> aplicar_heuristica(Resolucao::Vizinhanca llh,
                                                const Solucao& solucao);
    std::unique_ptr<Solucao> first_improvement(const Solucao& solucao) const;

    std::unique_ptr<Solucao> ejection_chains(const Solucao& solucao);

    boost::optional<Time_slot> ejection_move(Solucao& solucao, Time_slot place) const;

    Time_slot pick_place(const Solucao& solucao) const;

    boost::optional<Time_slot> pick_event_and_move(Solucao& solucao, Time_slot slot) const;

    std::vector<Event> list_all_liebhabers(const Solucao& solucao, Time_slot slot) const;

    boost::optional<Time_slot> choose_and_move(
        Solucao& solucao, const std::vector<Event>& liebhabers,
        Time_slot dest) const;

    Resolucao&             res;
    const long long        tempo_total;
    const long long        tempo_mutation;
    const long long        tempo_hill;
    const int              max_level;
    const std::vector<int> thresholds;
    const int              it_hc;
    int                    maior_fo;
    long long              tempo_fo;

    const std::array<Resolucao::Vizinhanca, 6> heuristicas_mutacionais{
        Resolucao::Vizinhanca::ES,
        Resolucao::Vizinhanca::EM,
        Resolucao::Vizinhanca::RS,
        Resolucao::Vizinhanca::RM
    };

    const std::vector<Resolucao::Vizinhanca> heuristicas_hill{
        Resolucao::Vizinhanca::HC_FI,
        Resolucao::Vizinhanca::HC_EC
    };
};

HySST::Impl::Impl(Resolucao& res, long long tempo_total, long long tempo_mutation,
             long long tempo_hill, int max_level, int t_start, int t_step,
             int it_hc)
    : res(res),
      tempo_total(tempo_total),
      tempo_mutation(tempo_mutation),
      tempo_hill(tempo_hill),
      max_level(max_level),
      thresholds(gen_thresholds(max_level, t_start, t_step)),
      it_hc(it_hc),
      maior_fo(),
      tempo_fo() {}


std::unique_ptr<Solucao> HySST::gerar_horario(const Solucao& s_inicial)
{
    auto t_total = Timer();
    auto s_atual = s_inicial.clone();
    auto s_best = s_inicial.clone();
    auto level = 0;

    d_->maior_fo = s_best->getFO();
    d_->tempo_fo = t_total.elapsed();

    while (t_total.elapsed() < d_->tempo_total) {
        auto s_stage_best = s_atual->clone();
        auto s_stage_start = s_atual->clone();
        auto eps = d_->thresholds[level];

        auto t_mu = Timer();
        while (t_mu.elapsed() < d_->tempo_mutation && t_total.elapsed() < d_->tempo_total) {
            auto llh = d_->choose_mut();
            auto s_viz = d_->aplicar_heuristica(llh, *s_atual);

            if (s_viz->getFO() > s_best->getFO()) {
                s_best = s_viz->clone();
                d_->maior_fo = s_best->getFO();
                d_->tempo_fo = t_total.elapsed();
            }
            if (s_viz->getFO() + eps > s_stage_best->getFO()) {
                s_stage_best = s_viz->clone();
            }
            if (s_viz->getFO() + eps > s_atual->getFO()) {
                s_atual = s_viz->clone();
            }
        }

        auto t_hc = Timer();
        if (s_stage_best->getFO() <= s_stage_start->getFO()) {
            while (t_hc.elapsed() < d_->tempo_hill && t_total.elapsed() < d_->tempo_total) {

                auto llh = d_->choose_hill();
                auto s_viz = d_->aplicar_heuristica(llh, *s_atual);

                if (s_viz->getFO() > s_stage_best->getFO()) {
                    s_stage_best = s_viz->clone();
                }
                if (s_viz->getFO() > s_best->getFO()) {
                    s_best = s_viz->clone();
                    d_->maior_fo = s_best->getFO();
                    d_->tempo_fo = t_total.elapsed();
                }
                s_atual = std::move(s_viz);
            }
        }

        if (s_stage_best->getFO() <= s_stage_start->getFO()) {
            if (level == d_->max_level - 1) {
                s_atual = std::move(s_stage_start);
                level = 0;
            } else {
                level++;
            }
        }
    }

    return s_best;
}

std::vector<int> HySST::Impl::gen_thresholds(int max_level, int t_start, int t_step)
{
    std::vector<int> thresholds(max_level);
    auto curr_t = t_start;

    for (auto i = 0; i < max_level; i++) {
        thresholds[i] = curr_t;
        curr_t += t_step;
    }

    return thresholds;
}

std::unique_ptr<Solucao> HySST::Impl::aplicar_heuristica(
    Resolucao::Vizinhanca llh,
    const Solucao& solucao
)
{
    switch (llh) {
        case Resolucao::Vizinhanca::ES: return res.event_swap(solucao);
        case Resolucao::Vizinhanca::EM: return res.event_move(solucao);
        case Resolucao::Vizinhanca::RS: return res.resource_swap(solucao);
        case Resolucao::Vizinhanca::RM: return res.resource_move(solucao);
        case Resolucao::Vizinhanca::HC_FI: return first_improvement(solucao);
        case Resolucao::Vizinhanca::HC_EC: return ejection_chains(solucao);
        default: return solucao.clone();
    }
}

std::unique_ptr<Solucao> HySST::Impl::first_improvement(
    const Solucao& solucao
) const
{
    auto s = solucao.clone();

    for (auto i = 0; i < it_hc; i++) {
        auto s_viz = [&] {
            if (Util::randomDouble() < 0.5) {
                return res.permute_resources(*s);
            } else {
                return res.kempe_move(*s);
            }
        }();

        if (s_viz->getFO() > s->getFO()) {
            s = std::move(s_viz);
        }
    }

    return s;
}

std::unique_ptr<Solucao> HySST::Impl::ejection_chains(const Solucao& solucao)
{
    auto s = solucao.clone();
    auto slot = pick_place(*s);

    for (auto i = 0; i < it_hc; i++) {
        auto cur = s->clone();
        if (auto nboostt = ejection_move(*cur, slot)) {
            slot = *nboostt;
            s = move(cur);
        } else {
            slot = pick_place(*s);
        }
    }

    res.gerarGrade(s.get());
    return s;
}

boost::optional<HySST::Impl::Time_slot> HySST::Impl::ejection_move(
    Solucao& solucao,
    Time_slot place
) const
{
    auto slot = pick_event_and_move(solucao, place);
    if (!slot) {
        return boost::none;
    }

    auto liebhabers = list_all_liebhabers(solucao, *slot);
    return choose_and_move(solucao, liebhabers, *slot);
}

HySST::Impl::Time_slot HySST::Impl::pick_place(const Solucao& solucao) const
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

boost::optional<HySST::Impl::Time_slot> HySST::Impl::pick_event_and_move(
    Solucao& solucao,
    Time_slot slot
) const
{
    auto& horario = solucao.getHorario();

    int dia, bloco, camada;
    std::tie(dia, bloco, camada) = slot;

    auto novo_dia = Util::randomBetween(0, dias_semana_util);
    auto novo_bloco = 2 * Util::randomBetween(0, res.getBlocosTamanho()/2);

    auto pd1 = horario.at(novo_dia, novo_bloco, camada);
    auto pd2 = horario.at(novo_dia, novo_bloco+1, camada);
    horario.clearSlot(novo_dia, novo_bloco, camada);
    horario.clearSlot(novo_dia, novo_bloco+1, camada);

    if (horario.insert(dia, bloco, camada, pd1)
        && horario.insert(dia, bloco+1, camada, pd2)) {
        return std::make_tuple(novo_dia, novo_bloco, camada);
    } else {
        return boost::none;
    }
}

std::vector<HySST::Impl::Event> HySST::Impl::list_all_liebhabers(
    const Solucao& solucao,
    Time_slot slot
) const
{
    int dia, bloco, camada;
    std::tie(dia, bloco, camada) = slot;

    std::vector<Event> liebhabers;

    auto periodo = solucao.camada_periodo.at(camada);
    auto disciplinas = res.getPeriodoXDisciplinas().at(periodo);
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

boost::optional<HySST::Impl::Time_slot> HySST::Impl::choose_and_move(
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
        return boost::none;
    }
}


Resolucao::Vizinhanca HySST::Impl::choose_hill() const
{
    /* First Improvement possui performance muito ruim
    auto llh = Util::randomChoice(heuristicas_hill);
    if (llh == Resolucao::Vizinhanca::HC_FI) {
    puts("first");
    } else {
    puts("ejec");
    }
    */
    return Resolucao::Vizinhanca::HC_EC;
}

Resolucao::Vizinhanca HySST::Impl::choose_mut() const
{
    return Util::randomChoice(heuristicas_mutacionais);
}

// Interface

HySST::HySST(Resolucao& res, long long tempo_total, long long tempo_mutation, long long tempo_hill, int max_level, int t_start, int t_step, int it_hc)
    : d_{std::make_unique<Impl>(res, tempo_total, tempo_mutation, tempo_hill,
                                max_level, t_start, t_step, it_hc)} {}

HySST::~HySST() {}

long long HySST::tempo_fo() const
{
    return d_->tempo_fo;
}

int HySST::maior_fo() const
{
    return d_->maior_fo;
}
