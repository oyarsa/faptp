#pragma once

#include <string>

class Configuracao
{
public:
    enum class TipoGrade
    {
        guloso,
        grasp,
        combinatorio,
        modelo
    };

    enum class TipoVizinhos
    {
        aleatorios,
        crescente
    };

    // Simples � operador novo, que s� cruza a camada inteira
    // Construtivo_reparo � o antigo
    // Substitui_bloco forma listas de disciplinas a serem trocadas, mantendo
    // blocos unidos (Tim�teo 2002)
    enum class TipoCruzamento
    {
        simples,
        construtivo_reparo,
        substitui_bloco,
        ordem,
        ciclo,
        pmx
    };

    // SubstDisc � o operador antigo, que troca disciplinas de lugar
    // SubstProf � o operador novo, que modifica quem leciona uma disciplina
    enum class TipoMutacao
    {
        substiui_disciplina,
        substitui_professor
    };

    enum class TipoFo
    {
        Soma_carga,
        Soft_constraints
    };

    enum class Versao_AG {
      Serial,
      Paralelo,
      MultiPopulacao
    };

    enum class Versao_GRASP {
      Serial,
      Paralelo,
      ProdutorConsumidor,
      Paralelo_ProdutoConsumidor
    };

    Configuracao() = default;
    Configuracao& arquivoEntrada(const std::string& filename);
    Configuracao& blocoTamanho(int n);
    Configuracao& camadaTamanho(int n);
    Configuracao& perfilTamanho(int n);
    Configuracao& tipoConstrucao(TipoGrade tipo);
    Configuracao& populacaoInicial(int n);
    Configuracao& numIteracoes(int n);
    Configuracao& numTorneioPares(int n);
    Configuracao& numTorneioPopulacao(int n);
    Configuracao& mutacaoProbabilidade(int p);
    Configuracao& tentativasMutacao(int n);
    Configuracao& graspVizinhanca(TipoVizinhos tipo);
    Configuracao& graspTempoConstrucao(int tempo);
    Configuracao& graspNumVizinhos(int n);
    Configuracao& graspAlfa(int alfa);
    Configuracao& porcentagemCruzamentos(int p);
    Configuracao& tipoCruzamento(TipoCruzamento tipo);
    Configuracao& tipoMutacao(TipoMutacao tipo);
    Configuracao& porcentagemSolucoesAleatorias(int n);
    Configuracao& tempoLimiteModelo(double tempo);
    Configuracao& numMaximoIteracoesSemEvolucaoAG(int n);
    Configuracao& numMaximoIteracoesSemEvolucaoGRASP(int n);
    Configuracao& timeout(long long t);
    Configuracao& tipoFo(TipoFo tipo);
    Configuracao& versaoAg(Versao_AG versao);
    Configuracao& versaoGrasp(Versao_GRASP versao);
    Configuracao& numThreadsAG(int n);
    Configuracao& numThreadsGRASP(int n);
    Configuracao& numParesProdutoConsumidor(int n);
private:
    friend class Resolucao;
    std::string filename_ = "input.json";
    int blocoTam_ = 4;
    int camadaTam_ = 20;
    int perfilTam_ = 600;
    TipoGrade tipoConstr_ = TipoGrade::grasp;
    int popInicial_ = 10;
    int numIter_ = 100;
    int numTorneioPares_ = 0;
    int numTorneioPop_ = 1;
    double mutProb_ = 0.2;
    int mutTentativas_ = 2;
    TipoVizinhos tipoVizinhanca_ = TipoVizinhos::aleatorios;
    double graspTempo_ = 0.001;
    int numVizinhos_ = 2;
    double graspAlfa_ = 0.3;
    double porcentCruz_ = 0.5;
    TipoCruzamento tipoCruz_ = TipoCruzamento::construtivo_reparo;
    TipoMutacao tipoMut_ = TipoMutacao::substiui_disciplina;
    double porcSolucoesAleatorias_ = 0.1;
    double tempoLimMod_ = 0;
    int numMaxIterSemEvoAG_ = 5;
    int numMaxIterSemEvoGRASP_ = 5;
    long long timeout_ = 60'000;
    TipoFo tipoFo_ = TipoFo::Soma_carga;
    Versao_AG versaoAg_ = Versao_AG::Serial;
    Versao_GRASP versaoGrasp_ = Versao_GRASP::Serial;
    int numThreadsAG_ = 1;
    int numThreadsGRASP_ = 1;
    int numParesProdutorConsumidor_ = 1;
};

inline Configuracao& Configuracao::arquivoEntrada(const std::string& filename)
{
    filename_ = filename;
    return *this;
}

inline Configuracao& Configuracao::blocoTamanho(int n)
{
    blocoTam_ = n;
    return *this;
}

inline Configuracao& Configuracao::camadaTamanho(int n)
{
    camadaTam_ = n;
    return *this;
}

inline Configuracao& Configuracao::perfilTamanho(int n)
{
    perfilTam_ = n;
    return *this;
}

inline Configuracao& Configuracao::tipoConstrucao(TipoGrade tipo)
{
    tipoConstr_ = tipo;
    return *this;
}

inline Configuracao& Configuracao::populacaoInicial(int n)
{
    popInicial_ = n;
    return *this;
}

inline Configuracao& Configuracao::numIteracoes(int n)
{
    numIter_ = n;
    return *this;
}

inline Configuracao& Configuracao::numTorneioPares(int n)
{
    numTorneioPares_ = n;
    return *this;
}

inline Configuracao& Configuracao::numTorneioPopulacao(int n)
{
    numTorneioPop_ = n;
    return *this;
}

inline Configuracao& Configuracao::mutacaoProbabilidade(int p)
{
    mutProb_ = p / 100.0;
    return *this;
}

inline Configuracao& Configuracao::tentativasMutacao(int n)
{
    mutTentativas_ = n;
    return *this;
}

inline Configuracao& Configuracao::graspVizinhanca(TipoVizinhos tipo)
{
    tipoVizinhanca_ = tipo;
    return *this;
}

inline Configuracao& Configuracao::graspTempoConstrucao(int tempo)
{
    graspTempo_ = tempo / 1000.0;
    return *this;
}

inline Configuracao& Configuracao::graspNumVizinhos(int n)
{
    numVizinhos_ = n;
    return *this;
}

inline Configuracao& Configuracao::graspAlfa(int alfa)
{
    graspAlfa_ = alfa / 100.0;
    return *this;
}

inline Configuracao& Configuracao::porcentagemCruzamentos(int p)
{
    porcentCruz_ = p / 100.0;
    return *this;
}

inline Configuracao& Configuracao::tipoCruzamento(TipoCruzamento tipo)
{
    tipoCruz_ = tipo;
    return *this;
}

inline Configuracao& Configuracao::tipoMutacao(TipoMutacao tipo)
{
    tipoMut_ = tipo;
    return *this;
}

inline Configuracao& Configuracao::porcentagemSolucoesAleatorias(int n)
{
    porcSolucoesAleatorias_ = n / 100.;
    return *this;
}

inline Configuracao& Configuracao::tempoLimiteModelo(double tempo)
{
    tempoLimMod_ = tempo;
    return *this;
}

inline Configuracao& Configuracao::numMaximoIteracoesSemEvolucaoAG(int n)
{
    numMaxIterSemEvoAG_ = n;
    return *this;
}

inline Configuracao& Configuracao::numMaximoIteracoesSemEvolucaoGRASP(int n)
{
    numMaxIterSemEvoGRASP_ = n;
    return *this;
}

inline Configuracao& Configuracao::timeout(long long t)
{
    timeout_ = t;
    return *this;
}

inline Configuracao& Configuracao::tipoFo(TipoFo tipo)
{
    tipoFo_ = tipo;
    return *this;
}

inline Configuracao&
Configuracao::versaoAg(Configuracao::Versao_AG versao)
{
  versaoAg_ = versao;
  return *this;
}

inline Configuracao&
Configuracao::versaoGrasp(Configuracao::Versao_GRASP versao)
{
  versaoGrasp_ = versao;
  return *this;
}

inline Configuracao&
Configuracao::numThreadsAG(int n)
{
    numThreadsAG_ = n;
    return *this;
}

inline Configuracao&
Configuracao::numThreadsGRASP(int n)
{
    numThreadsGRASP_ = n;
    return *this;
}

inline Configuracao&
Configuracao::numParesProdutoConsumidor(int n)
{
    numParesProdutorConsumidor_ = n;
    return *this;
}
