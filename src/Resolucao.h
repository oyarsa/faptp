#ifndef RESOLUCAO_H
#define RESOLUCAO_H

#include <map>
#include <vector>
#include <memory>

#include <modelo-grade/aluno.h>
#include <modelo-grade/curso.h>
#include "includes/json/json.h"

#include "Professor.h"
#include "Disciplina.h"
#include "ProfessorDisciplina.h"
#include "AlunoPerfil.h"
#include "Solucao.h"
#include "Util.h"

class Resolucao;

class Configuracao
{
public:
	enum class TipoGrade
	{
		GULOSO,
		GRASP,
		COMBINATORIO,
		MODELO
	};
	enum class TipoVizinhos
	{
		ALEATORIOS,
		CRESCENTE
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
private:
	friend class Resolucao;
	std::string filename_ = "input.json";
	int blocoTam_ = 4;
	int camadaTam_ = 35;
	int perfilTam_ = 1413;
	TipoGrade tipoConstr_ = TipoGrade::GRASP;
	int popInicial_ = 10;
	int numIter_ = 100;
	int numTorneioPares_ = 0;
	int numTorneioPop_ = 1;
	double mutProb_ = 0.2;
	int mutTentativas_ = 2;
	TipoVizinhos tipoVizinhanca_ = TipoVizinhos::ALEATORIOS;
	double graspTempo_ = 0.001;
	int numVizinhos_ = 2;
	double graspAlfa_ = 0.3;
};

class Resolucao {
public:
    Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho,
			  Configuracao::TipoGrade pTipoConstrucao, std::string arquivoEntrada = "input.json");
	Resolucao(const Configuracao& c);
    virtual ~Resolucao();

    double start();
    double start(bool input);

    Solucao* gerarHorarioAG();

    double gerarGrade();

    void showResult();

    Solucao* getSolucao();

	fagoc::Curso& getCurso();
	const std::vector<fagoc::Aluno>& getAlunos() const;

	std::vector<std::vector<char>> converteHorario(Solucao *pSolucao);

    /*
     Parâmetros da execução da solução
     */
    // Horário população inicial
    int horarioPopulacaoInicial;
    int horarioProfessorColisaoMax = 2;

    // Horário torneio
    double horarioTorneioPares;
    double horarioTorneioPopulacao;

    // Horário cruzamento
    int horarioCruzamentoFilhos = 2;
    int horarioCruzamentoDias;
    double horarioCruzamentoCamadas;
    int horarioCruzamentoTentativasMax = 1;

    int horarioIteracao;

    // Horário mutação
    double horarioMutacaoProbabilidade;
    int horarioMutacaoTentativas;

    // Grade tipo de construção
    Configuracao::TipoGrade gradeTipoConstrucao;
    double gradeAlfa;

    // Grade GRASP
    Configuracao::TipoVizinhos gradeGraspVizinhanca;
    int gradeGraspVizinhos;
    double gradeGraspTempoConstrucao;

private:
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;
    std::string arquivoEntrada;
    std::map<std::string, Professor*> professores;
    std::vector<Disciplina*> disciplinas;
    std::map<std::string, int> disciplinasIndex;
    std::map< std::string, std::vector<Disciplina*> > periodoXdisciplina;
    std::map<std::string, AlunoPerfil*> alunoPerfis;
    std::map<std::string, ProfessorDisciplina*> professorDisciplinas;
    Solucao* solucao;
    Json::Value jsonRoot;
	Util util;
	std::unique_ptr<fagoc::Curso> curso;
	std::vector<fagoc::Aluno> alunos;

    void carregarDados();
    void carregarDadosProfessores();
    void carregarDadosDisciplinas();
    void carregarAlunoPerfis();
    void carregarDadosProfessorDisciplinas();
    void carregarSolucao();

    std::vector<Disciplina*> ordenarDisciplinas();
    std::vector<Disciplina*> ordenarDisciplinas(std::vector<Disciplina*> pDisciplina);
    void atualizarDisciplinasIndex();

    std::vector<Solucao*> gerarHorarioAGPopulacaoInicial();
    std::vector<Solucao*> gerarHorarioAGTorneioPar(std::vector<Solucao*> solucoesPopulacao);
    Solucao* gerarHorarioAGTorneio(std::vector<Solucao*> solucoesPopulacao) const;
    Solucao* gerarHorarioAGTorneio2(std::vector<Solucao*> solucoesPopulacao);
    std::vector<Solucao*> gerarHorarioAGCruzamentoAleatorio(Solucao *solucaoPai1, Solucao *solucaoPai2);
    bool gerarHorarioAGCruzamentoAleatorioReparoBloco(Solucao *&solucaoFilho, int diaG, int blocoG, int camadaG);
    bool gerarHorarioAGCruzamentoAleatorioReparo(Solucao *&solucaoFilho, int diaG, int blocoG, int camadaG);
    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*> &populacao);
    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*> &populacao, int populacaoMax);
    std::vector<Solucao*> gerarHorarioAGMutacao(std::vector<Solucao*> filhos);
    Solucao* gerarHorarioAGMutacao(Solucao* pSolucao);

    double gerarGrade(Solucao *&pSolucao);

    double gerarGradeTipoGuloso(Solucao *&pSolucao);

    Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, std::vector<Disciplina*> disciplinasRestantes, int maxDeep, int deep, int current);
    Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, std::vector<Disciplina*> disciplinasRestantes, int maxDeep);
    double gerarGradeTipoCombinacaoConstrutiva(Solucao *&pSolucao);

    void gerarGradeTipoGraspConstrucao(Solucao *pSolucao);

    void gerarGradeTipoGraspConstrucao(Grade* pGrade);
    void gerarGradeTipoGraspConstrucao(Grade* pGrade, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
    Solucao* gerarGradeTipoGraspRefinamentoAleatorio(Solucao *pSolucao);
    Solucao* gerarGradeTipoGraspRefinamentoCrescente(Solucao *pSolucao);
    double gerarGradeTipoGrasp();
    double gerarGradeTipoGrasp(Solucao *&pSolucao);

    double gerarGradeTipoGraspClear(Solucao *&pSolucao);

	double gerarGradeTipoModelo(Solucao *pSolucao);

    std::vector<Disciplina*>::iterator getLimiteIntervaloGrasp(std::vector<Disciplina*> pApRestante);
    int getIntervaloAlfaGrasp(std::vector<Disciplina*> pApRestante) const;

    void showResult(Solucao *pSolucao);
};

#endif /* RESOLUCAO_H */