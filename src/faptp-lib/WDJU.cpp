#include <faptp-lib/WDJU.h>

#include <gsl/gsl>

#include <faptp-lib/Timer.h>

WDJU::WDJU(const Resolucao& res, long long timeout, int stagnation_limit,
           double jump_factor)
    : res_(res),
      timeout_(timeout),
      stagnation_limit_(stagnation_limit),
      max_jump_factor_(jump_factor),
      tempo_fo_(),
      maior_fo_() {}

std::unique_ptr<Solucao> WDJU::gerar_horario(const Solucao& solucao)
{
    auto jump_height = 0;
    std::vector<double> history{};
    auto jump_factor = next_jump(history);

    auto s_best = solucao.clone();
    auto s_atual = solucao.clone();

    auto iter = 0;
    Timer t;

    maior_fo_ = s_best->getFO();
    tempo_fo_ = t.elapsed();

    while (t.elapsed() < timeout_) {
        auto s_viz = gerar_vizinho(*s_atual);

        if (s_viz->getFO() >= s_atual->getFO() - jump_height) {
            iter = 0;
            jump_height--;
            s_atual = move(s_viz);

            if (s_atual->getFO() > s_best->getFO()) {
                s_best = s_atual->clone();

                history.push_back(jump_factor);
                jump_factor = 1;

                maior_fo_ = s_best->getFO();
                tempo_fo_ = t.elapsed();
            }
        } else {
            iter++;
            if (iter == stagnation_limit_) {
                iter = 0;
                jump_height = gsl::narrow_cast<int>(s_best->getFO() * jump_factor);
                jump_factor = next_jump(history);
            }
        }
    }

    return s_best;
}

double WDJU::next_jump(const std::vector<double>& history) const
{
    if (history.empty() || Util::randomDouble() < 0.5) {
        return Util::randomDouble() * max_jump_factor_;
    } else {
        return Util::randomChoice(history);
    }
}

std::unique_ptr<Solucao> WDJU::gerar_vizinho(const Solucao& solucao) const
{
    const std::vector<Resolucao::Vizinhanca> movimentos{
        Resolucao::Vizinhanca::ES,
        Resolucao::Vizinhanca::EM,
        Resolucao::Vizinhanca::RS,
        Resolucao::Vizinhanca::RM
    };

    switch (Util::randomChoice(movimentos)) {
    case Resolucao::Vizinhanca::ES: return res_.event_swap(solucao);
    case Resolucao::Vizinhanca::EM: return res_.event_move(solucao);
    case Resolucao::Vizinhanca::RS: return res_.resource_swap(solucao);
    case Resolucao::Vizinhanca::RM: return res_.resource_move(solucao);
    default: return std::make_unique<Solucao>(solucao);
    }
}

long long WDJU::tempo_fo() const
{
    return tempo_fo_;
}

Solucao::FO_t WDJU::maior_fo() const
{
    return maior_fo_;
}
