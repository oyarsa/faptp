#include <random>
#include <chrono>

#include <gsl.h>

#include "Aleatorio.h"

class Aleatorio
{
    //! Alias para o tipo de valor que a engine mt19337 toma como seed
    using TipoSeed = std::mt19937::result_type;
    //! Alias para a classe que representa um relogio de alta precisao
    using TipoRelogio = std::chrono::high_resolution_clock;
public:
    Aleatorio();
    Aleatorio& operator=(const Aleatorio&) = delete;
    Aleatorio(const Aleatorio&) = delete;

    //! \brief Gera um numero aleatorio de 0 a 32767 a partir de uma distribuicao uniforme
    //! \return Int na faixa 0 <= x <= 32767
    int randomInt();

    //!brief Retorna uma instancia do Mersenne-Twister apropriadamente seedada
    //! com um relogio de alta resolucao
    //! \return Instancia do mt19937 seedado
    std::mt19937 geradorAleatorio();
private:
    //! Constante para maior valor a ser gerado. Igual ao RAND_MAX padrao
    const int max_random_ = 32767;
    //! Gera uma distribuicao uniforme na mesma faixa do rand()
    std::uniform_int_distribution<> dist_;
    //! Seed do mersenne twister. Numero de milissegundos, em alta precisao,
    //! desde Epoch
    TipoSeed seed_;
    //! Engine de numeros aleatorios
    std::mt19937 gerador_;
};

Aleatorio::Aleatorio() : dist_ {
                         0, max_random_
                         },
                         seed_ {gsl::narrow_cast<TipoSeed>(
                             TipoRelogio::now().time_since_epoch().count())},
                         gerador_ {seed_} {}

std::mt19937 Aleatorio::geradorAleatorio()
{
    return gerador_;
}

int Aleatorio::randomInt()
{
    return dist_(gerador_);
}

int aleatorio::randomInt()
{
    static Aleatorio a;
    return a.randomInt();
}

