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
	// Simples é operador novo, que só cruza a camada inteira
	// Construtivo_reparo é o antigo
	enum class TipoCruzamento
	{
		simples,
		construtivo_reparo
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
private:
	friend class Resolucao;
	std::string filename_ = "input.json";
	int blocoTam_ = 4;
	int camadaTam_ = 35;
	int perfilTam_ = 1413;
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
	double porcentCruz_;
	TipoCruzamento tipoCruz_ = TipoCruzamento::construtivo_reparo;
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
