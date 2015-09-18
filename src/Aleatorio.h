#include <random>
#include <chrono>

class Aleatorio {
    //! Alias para o tipo de valor que a engine mt19337 toma como seed
    using TipoSeed = std::mt19937::result_type;
    //! Alias para a classe que representa um relogio de alta precisao
    using TipoRelogio = std::chrono::high_resolution_clock;
public:
    Aleatorio();
    //! \brief Gera um numero aleatorio de 0 a 32767 a partir de uma distribuicao uniforme
    //! \return Int na faixa 0 <= x <= 32767
    int randomInt();

    //!brief Retorna uma instancia do Mersenne-Twister apropriadamente seedada
    //! com um relogio de alta resolucao
    //! \return Instancia do mt19937 seedado
    std::mt19937 geradorAleatorio();
private:
    //! Constante para maior valor a ser gerado. Igual ao RAND_MAX padrao
    const int MAX_RANDOM = 32767;
    //! Gera uma distribuicao uniforme na mesma faixa do rand()
    std::uniform_int_distribution<> dist;
    //! Seed do mersenne twister. Numero de milissegundos, em alta precisao,
    //! desde Epoch
    TipoSeed seed;
    //! Engine de numeros aleatorios
    std::mt19937 gerador;
};
