#pragma once

#include <chrono>
#include <ostream>

/// Implementa um cronômetro, facilitando a medição da passagem
/// do tempo na condição de parada dos algoritmos.
class Timer
{
public:
  /// Cria um novo timer, com tempo inicial igual ao momento de criação.
  Timer()
  {
    reset();
  }

  /// Reseta o tempo inicial para o momento da chamada.
  void
  reset()
  {
    start_ = std::chrono::high_resolution_clock::now();
  }

  /// Calcula o tempo decorrido (em ms) do momento inicial ao momento da chamada.
  ///
  /// @return Tempo em milissegundos entre o momento inicial e o "agora".
  long long
  elapsed() const
  {
    auto e = std::chrono::high_resolution_clock::now() - start_;
    return std::chrono::duration_cast<std::chrono::milliseconds>(e).count();
  }

  /// Imprime a representação textual do tempo decorrido para a `os`,
  /// exemplo: `150ms`.
  ///
  /// @param os Stream de saída onde será impressa a representação textual.
  /// @param t Timer que será representado.
  /// @return A stream de saída `os`, após ser modificada.
  friend std::ostream&
  operator<<(std::ostream& os, const Timer& t)
  {
    os << t.elapsed() << "ms";
    return os;
  }

private:
  /// Momento inicial, de onde os cálculos de tempo decorrido serão derivados.
  std::chrono::high_resolution_clock::time_point start_;
};
