#include "SA.h"
#include "Timer.h"

SA::SA(const Resolucao& res, double alfa, double t0, int max_iter, int max_reheats,
       long long timeout,
       const std::vector<std::pair<Resolucao::Vizinhanca, int>>& chances)
    : res_(res),
      alfa_(alfa),
      t0_(t0),
      max_iter_(max_iter),
      max_reheats_(max_reheats),
      timeout_(timeout),
      chances_vizinhancas_(Util::gen_chance_array(chances)) {}

std::unique_ptr<Solucao> SA::gerar_horario(const Solucao& s_inicial) const
{
    auto s_atual = std::make_unique<Solucao>(s_inicial);
    auto s_best = std::make_unique<Solucao>(s_inicial);

    auto temp = t0_;
    auto reheats = 0;
    auto tempoInicial = Util::now();
    Timer t;

    while (reheats < max_reheats_ && t.elapsed() < timeout_) {
        for (auto i = 0; i < max_iter_ && t.elapsed() < timeout_; i++) {
            auto s_viz = gerar_vizinho(*s_atual);
            auto delta = s_viz->getFO() - s_atual->getFO();

            if (delta > 0 || Util::randomDouble() < std::exp(-delta / temp)) {
                s_atual = std::move(s_viz);

                if (s_atual->getFO() > s_best->getFO()) {
                    s_best = std::make_unique<Solucao>(*s_atual);
                }
            }
        }

        temp *= alfa_;
        if (temp < 0.1) {
            reheats++;
            temp = t0_;
        }
    }

    return s_best;
}

std::unique_ptr<Solucao> SA::gerar_vizinho(const Solucao& solucao) const
{
    switch (escolher_vizinhanca()) {
        case Resolucao::Vizinhanca::ES: return res_.event_swap(solucao);
        case Resolucao::Vizinhanca::EM: return res_.event_move(solucao);
        case Resolucao::Vizinhanca::RS: return res_.resource_swap(solucao);
        case Resolucao::Vizinhanca::RM: return res_.resource_move(solucao);
        case Resolucao::Vizinhanca::KM: return res_.kempe_move(solucao);
        default: return std::make_unique<Solucao>(solucao);
    }
}

Resolucao::Vizinhanca SA::escolher_vizinhanca() const
{
    return Util::randomChoice(chances_vizinhancas_);
}
