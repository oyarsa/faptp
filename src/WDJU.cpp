#include "WDJU.h"
#include "gsl/gsl"


WDJU::WDJU(const Resolucao& res, long long timeout, int stagnation_limit, 
           double jump_factor) 
    : res_(res),
      timeout_(timeout),
      stagnation_limit_(stagnation_limit),
      max_jump_factor_(jump_factor) {}

std::unique_ptr<Solucao> WDJU::gerar_horario(const Solucao& solucao) const
{
    int jump_height{0};
    double jump_factor{0.005};

    auto s_best = std::make_unique<Solucao>(solucao);
    auto s_atual = std::make_unique<Solucao>(solucao);
    std::vector<double> history{};
    auto iter = 0;

    auto comeco = Util::now();
    while (Util::chronoDiff(Util::now(), comeco) < timeout_) {
        auto viz = gerar_vizinho(*s_atual);
        printf("Jump: %d\n", jump_height);
        printf("Fo: %d\n", viz->getFO());
        if (viz->getFO() > s_atual->getFO() - jump_height) {
            iter = 0;
            jump_height--;
            s_atual = move(viz);

            if (s_atual->getFO() > s_best->getFO()) {
                printf("Melhoria: %d\n", s_atual->getFO());

                s_best = std::make_unique<Solucao>(*s_atual);

                history.push_back(jump_factor);
                jump_height = 0;
                jump_factor = next_jump(history);
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
