#include "WDJU.h"
#include "gsl/gsl"
#include "Timer.h"

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
    int jump_height{0};
    double jump_factor{0.005};

    auto s_best = std::make_unique<Solucao>(solucao);
    auto s_atual = std::make_unique<Solucao>(solucao);

    std::vector<double> history{};
    auto iter = 0;
    Timer t;

    maior_fo_ = s_best->getFO();
    tempo_fo_ = t.elapsed();

    while (t.elapsed() < timeout_) {
        auto viz = gerar_vizinho(*s_atual);
        if (viz->getFO() > s_atual->getFO() - jump_height) {
            iter = 0;
            jump_height--;
            s_atual = move(viz);

            if (s_atual->getFO() > s_best->getFO()) {
                s_best = std::make_unique<Solucao>(*s_atual);

                history.push_back(jump_factor);
                jump_height = 0;
                jump_factor = next_jump(history);

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
        Resolucao::Vizinhanca::RM,
        Resolucao::Vizinhanca::KM
    };

    switch (Util::randomChoice(movimentos)) {
    case Resolucao::Vizinhanca::ES: return res_.event_swap(solucao);
    case Resolucao::Vizinhanca::EM: return res_.event_move(solucao);
    case Resolucao::Vizinhanca::RS: return res_.resource_swap(solucao);
    case Resolucao::Vizinhanca::RM: return res_.resource_move(solucao);
    case Resolucao::Vizinhanca::KM: return res_.kempe_move(solucao);
    default: return std::make_unique<Solucao>(solucao);
    }
}

long long WDJU::tempo_fo() const
{
    return tempo_fo_;
}

int WDJU::maior_fo() const
{
    return maior_fo_;
}
