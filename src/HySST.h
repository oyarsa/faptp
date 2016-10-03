#pragma once

#include "Resolucao.h"
#include <algorithm>
#include <optional/optional.hpp>

namespace ex = std::experimental;

class HySST
{
public:
    HySST(const Resolucao& res, long long tempo_total, 
          long long tempo_mutation, long long tempo_hill, 
          int max_level, int t_start, int t_step, int it_hc);

    std::unique_ptr<Solucao> gerar_horario(const Solucao& s_inicial) const;

    const Resolucao& res() const;
    long long tempo_total() const;
    long long tempo_mutation() const;
    long long tempo_hill() const;
    int max_level() const;

private:
    using Time_slot = std::tuple<int, int, int>;
    using Event = std::pair<Time_slot, ProfessorDisciplina*>;

    static std::vector<int> gen_thresholds(int max_level, int t_start, int t_step);

    Resolucao::Vizinhanca choose_hill() const;

    Resolucao::Vizinhanca choose_mut() const;

    std::unique_ptr<Solucao> aplicar_heuristica(Resolucao::Vizinhanca llh, 
                                                const Solucao& solucao) const;
    std::unique_ptr<Solucao> first_improvement(const Solucao& solucao) const;

    std::unique_ptr<Solucao> ejection_chains(const Solucao& solucao) const;

    ex::optional<Time_slot> ejection_move(Solucao& solucao, Time_slot place) const;

    Time_slot pick_place(const Solucao& solucao) const;

    ex::optional<Time_slot> pick_event_and_move(Solucao& solucao, Time_slot slot) const;

    std::vector<Event> list_all_liebhabers(const Solucao& solucao, Time_slot slot) const;

    ex::optional<Time_slot> choose_and_move(
        Solucao& solucao, const std::vector<Event>& liebhabers, 
        Time_slot dest) const;

    const Resolucao&       res_;
    const long long        tempo_total_;
    const long long        tempo_mutation_;
    const long long        tempo_hill_;
    const int              max_level_;
    const std::vector<int> thresholds_;
    const int              it_hc_;

    const std::array<Resolucao::Vizinhanca, 6> heuristicas_mutacionais_{
        Resolucao::Vizinhanca::ES,
        Resolucao::Vizinhanca::EM,
        Resolucao::Vizinhanca::RS,
        Resolucao::Vizinhanca::RM
    };

    const std::vector<Resolucao::Vizinhanca> heuristicas_hill_{
        Resolucao::Vizinhanca::HC_FI,
        Resolucao::Vizinhanca::HC_EC
    };
};
