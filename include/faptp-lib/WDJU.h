#pragma once

#include <memory>

#include <faptp-lib/Solucao.h>
#include <faptp-lib/Resolucao.h>

class WDJU
{
public:
  /// Constrói uma configuração do WDJU.
  ///
  /// @param res Objeto de onde os operadores de vizinhança serão chamados
  /// @param timeout Tempo limite de execução do algoritmo.
  /// @param stagnation_limit Limite de iterações sem melhoria.
  /// @param jump_factor Fator de alteração da altura do salto.
  WDJU(const Resolucao& res, long long timeout, int stagnation_limit,
       double jump_factor);

  /// Aplica o WDJU em uma solução inicial, buscando melhorar a solução.
  /// A melhor solução é retornada quando o tempo se esgota.
  ///
  /// @param solucao Solução inicial para o algoritmo.
  /// @return Melhor solução encontrada.
  std::unique_ptr<Solucao> gerar_horario(const Solucao& solucao);

  /// Obtém o tempo em milissegundos do começo da execução até encontrar
  /// a primeira solução com a melhor FO.
  ///
  /// @return Tempo em milissegundos até encontrar solução com a melhor FO.
  long long tempo_fo() const;

  /// Obtém FO da melhor solução encontrada.
  ///
  /// @return FO da melhor solução encontrada.
  Solucao::FO_t maior_fo() const;

private:
  double next_jump(const std::vector<double>& history) const;
  std::unique_ptr<Solucao> gerar_vizinho(const Solucao& solucao) const;

  const Resolucao& res_;
  long long timeout_;
  int stagnation_limit_;
  double max_jump_factor_;
  long long tempo_fo_;
  Solucao::FO_t maior_fo_;
};
