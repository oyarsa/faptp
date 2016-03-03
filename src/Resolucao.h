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
#include "Configuracao.h"

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

	// Converte a matriz tridimensional do horário em uma matriz bidimensional
	// de binários para o modelo
	std::vector<std::vector<char>> converteHorario(Solucao *pSolucao) const;
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
	double horarioCruzamentoPorcentagem = 0;
    int horarioCruzamentoFilhos = 2;
    int horarioCruzamentoDias;
    double horarioCruzamentoCamadas;
    int horarioCruzamentoTentativasMax = 1;
	// Operador de cruzamento selecionado
	Configuracao::TipoCruzamento horarioTipoCruzamento;
	// Operador de mutação selecionado
	Configuracao::TipoMutacao horarioTipoMutacao;

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
	
	// Solução alvo, a iteração em que ela foi alcançada primeiro e o tempo
	double foAlvo;
	int iteracaoAlvo;
	long long tempoAlvo;

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
	std::vector<Solucao*> gerarHorarioAGCruzamento(const std::vector<Solucao*>& parVencedor);
	std::vector<Solucao*> gerarHorarioAGPopulacaoInicial();
    std::vector<Solucao*> gerarHorarioAGTorneioPar(std::vector<Solucao*> solucoesPopulacao);
    Solucao* gerarHorarioAGTorneio(std::vector<Solucao*> solucoesPopulacao) const;
    Solucao* gerarHorarioAGTorneio2(std::vector<Solucao*> solucoesPopulacao);

    std::vector<Solucao*> gerarHorarioAGCruzamentoConstrutivoReparo(Solucao *solucaoPai1, Solucao *solucaoPai2);
    bool gerarHorarioAGCruzamentoAleatorioReparoBloco(Solucao *&solucaoFilho, int diaG, int blocoG, int camadaG);
    bool gerarHorarioAGCruzamentoAleatorioReparo(Solucao *&solucaoFilho, int diaG, int blocoG, int camadaG);

	void cruzaCamada(Solucao*& filho, const Solucao* pai, int camada) const;
	std::vector<Solucao*> gerarHorarioAGCruzamentoSimples(Solucao *solucaoPai1, Solucao *solucaoPai2);

	std::vector<Solucao*> gerarHorarioAGCruzamentoSubstBloco(Solucao *solucaoPai1, Solucao *solucaoPai2);

    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*> &populacao);
    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*> &populacao, int populacaoMax);
    std::vector<Solucao*> gerarHorarioAGMutacao(std::vector<Solucao*> filhos);
    Solucao* gerarHorarioAGMutacaoSubstDisc(Solucao* pSolucao);
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

	Solucao* gerarHorarioAGMutacaoSubstProf(const Solucao* pSolucao) const;

	void logPopulacao(const std::vector<Solucao*>& populacao) const;

	std::vector<Solucao*> gerarHorarioAGPopulacaoInicial2();
};

#endif /* RESOLUCAO_H */
