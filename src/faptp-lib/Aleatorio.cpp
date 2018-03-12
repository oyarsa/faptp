#include <vector>
#include <random>
#include <chrono>
#include <omp.h>
#include <cstdint>

#include <faptp-lib/Aleatorio.h>

class Aleatorio
{
    //! Alias para a classe que representa um relogio de alta precisao
    using Relogio = std::chrono::high_resolution_clock;
public:
    Aleatorio();
    Aleatorio& operator=(const Aleatorio&) = delete;
    Aleatorio(const Aleatorio&) = delete;

    //! \brief Gera um numero aleatorio de 0 a 32767 a partir de uma distribuicao uniforme
    //! \return Int na faixa 0 <= x <= 32767
    int randomInt();

private:
    uint32_t state_;
};

//! Constante para maior valor a ser gerado. Igual ao RAND_MAX padrao
constexpr uint32_t max_random = 32767;

Aleatorio::Aleatorio() 
{
  const auto now = Relogio::now().time_since_epoch().count();
  const auto thread_number = omp_get_thread_num();
  state_ = static_cast<uint32_t>(now + thread_number);
}

int Aleatorio::randomInt()
{
  uint32_t x = state_;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  state_ = x;
  return x & max_random;
}

static std::vector<Aleatorio> generators;

void aleatorio::initRandom(int numThreads)
{
  generators = std::vector<Aleatorio>(numThreads);
}

int aleatorio::randomInt()
{
  const auto thread_number = omp_get_thread_num();
  return generators[thread_number].randomInt();
}

