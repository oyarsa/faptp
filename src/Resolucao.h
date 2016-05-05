#ifndef RESOLUCAO_H
#define RESOLUCAO_H

#include <map>
#include <vector>
#include <memory>
#include <set>

#ifdef MODELO
	#include <modelo-grade/aluno.h>
	#include <modelo-grade/curso.h>
#endif

#include "includes/json/json.h"

#include "Algorithms.h"
#include "Professor.h"
#include "Disciplina.h"
#include "ProfessorDisciplina.h"
#include "AlunoPerfil.h"
#include "Solucao.h"
#include "Aleatorio.h"
#include "Configuracao.h"

class Resolucao {
public:
    Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho,
			  Configuracao::TipoGrade pTipoConstrucao, 
			  std::string arquivoEntrada = "input.json");
	Resolucao(const Configuracao& c);
    virtual ~Resolucao();

    double start();
    double start(bool input);

    Solucao* gerarHorarioAG();
    Solucao* gerarHorarioAG2();
    Solucao* gerarHorarioAG3();

    double gerarGrade();

    void showResult();

    Solucao* getSolucao();

	void teste();

#ifdef MODELO
	fagoc::Curso& getCurso();
	const std::vector<fagoc::Aluno>& getAlunos() const;
#endif
	std::string getLog() const;

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
	std::size_t hashAlvo;

	void logExperimentos();

	// Porcentagem de soluções aleatórias que serão criadas e adicionadas à população
	// à cada iteração
	double porcentagemSolucoesAleatorias;
	
	// Número de iterações máximo que o AG continuará sem evoluir a solução
	int numMaximoIteracoesSemEvolucaoAG;
	int ultimaIteracao;
	// Número de iterações máximo que o GRASP continuará sem evoluir a grade
	int maxIterSemEvoGrasp;

	static const int numcruz = 6;
	int contadorIguaisCruz[numcruz];
	int contadorIguaisMut[2];
	int contadorMelhoresCruz[numcruz];
	int contadorMelhoresMut[2];
	long long tempoTotalCruz[numcruz];
	long long tempoTotalMut[2];
	long long tempoTotalSelec;
	long long tempoTotalElit;
private:
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;
    std::string arquivoEntrada;
    std::unordered_map<std::string, Professor*> professores;
    std::vector<Disciplina*> disciplinas;
    std::unordered_map<std::string, int> disciplinasIndex;
    std::map<std::string, std::vector<Disciplina*>> periodoXdisciplina;
    std::unordered_map<std::string, AlunoPerfil*> alunoPerfis;
    std::unordered_map<std::string, ProfessorDisciplina*> professorDisciplinas;
    Solucao* solucao;
    Json::Value jsonRoot;
#ifdef MODELO
	std::unique_ptr<fagoc::Curso> curso;
	std::vector<fagoc::Aluno> alunos;
#endif
	std::chrono::system_clock::time_point tempoInicio;
	double tempoLimiteModelo;
	std::ostringstream log;

    void carregarDados();
    void carregarDadosProfessores();
    void carregarDadosDisciplinas();
    void carregarAlunoPerfis();
    void carregarDadosProfessorDisciplinas();
    void carregarSolucao();

	Disciplina* getDisciplinaByName(const std::string& nomeDisc);
    std::vector<Disciplina*> ordenarDisciplinas();
    std::vector<Disciplina*> ordenarDisciplinas(std::vector<Disciplina*> pDisciplina);
    void atualizarDisciplinasIndex();
	std::vector<Solucao*> gerarHorarioAGCruzamento(const std::vector<Solucao*>& parVencedor);
	std::vector<Solucao*> gerarHorarioAGCruzamentoExper(const std::vector<Solucao*>& parVencedor,
													Configuracao::TipoCruzamento tipoCruz);
	std::vector<Solucao*> gerarHorarioAGPopulacaoInicial();
    std::vector<Solucao*> gerarHorarioAGTorneioPar(std::vector<Solucao*> solucoesPopulacao);
    Solucao* gerarHorarioAGTorneio(std::vector<Solucao*> solucoesPopulacao) const;
    Solucao* gerarHorarioAGTorneio2(std::vector<Solucao*> solucoesPopulacao);

    std::vector<Solucao*> gerarHorarioAGCruzamentoConstrutivoReparo(Solucao *solucaoPai1, Solucao *solucaoPai2);
    bool gerarHorarioAGCruzamentoAleatorioReparoBloco(Solucao *&solucaoFilho, int diaG, int blocoG, int camadaG);
    bool gerarHorarioAGCruzamentoAleatorioReparo(Solucao *&solucaoFilho, int diaG, int blocoG, int camadaG);

	int cruzaCamada(Solucao*& filho, const Solucao* pai, int camada) const;
	std::vector<Solucao*> gerarHorarioAGCruzamentoSimples(Solucao *pai1, Solucao *pai2);

	std::vector<Solucao*> gerarHorarioAGCruzamentoSubstBloco(Solucao *solucaoPai1, Solucao *solucaoPai2);

    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*> &populacao);
    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*> &populacao, int populacaoMax) const;
    std::vector<Solucao*> gerarHorarioAGMutacao(std::vector<Solucao*>& populacao);
    std::vector<Solucao*> gerarHorarioAGMutacaoExper(std::vector<Solucao*>& populacao,
												 Configuracao::TipoMutacao tipoMut);
    Solucao* gerarHorarioAGMutacaoSubstDisc(Solucao* pSolucao);
	bool swapBlocos(Solucao& solucao, int posX1, int posX2) const;
    Solucao* gerarHorarioAGMutacao(Solucao* pSolucao);

	double gerarGradeTipoGrasp2(Solucao* solucao);
	double gerarGrade(Solucao *pSolucao);

    double gerarGradeTipoGuloso(Solucao *&pSolucao);

    Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, int maxDeep, int deep, 
											   std::unordered_set<std::string>::const_iterator current);
    Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, int maxDeep);
    double gerarGradeTipoCombinacaoConstrutiva(Solucao *&pSolucao);

    void gerarGradeTipoGraspConstrucao(Solucao *pSolucao);

    void gerarGradeTipoGraspConstrucao(Grade* pGrade);
    void gerarGradeTipoGraspConstrucao(Grade* pGrade, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
    Solucao* gerarGradeTipoGraspRefinamentoAleatorio(Solucao *pSolucao);
    Solucao* gerarGradeTipoGraspRefinamentoCrescente(Solucao *pSolucao);
    double gerarGradeTipoGrasp();
    double gerarGradeTipoGrasp(Solucao *&pSolucao);

	double gerarGradeTipoModelo(Solucao *pSolucao);

    int getIntervaloAlfaGrasp(const std::vector<Disciplina*>& apRestante);

    void showResult(Solucao *pSolucao);

	Solucao* gerarHorarioAGMutacaoSubstProf(const Solucao& pSolucao) const;

	void logPopulacao(const std::vector<Solucao*>& populacao, int iter);

	std::vector<Solucao*> gerarHorarioAGPopulacaoInicial2();
	bool gerarCamada(Solucao* solucao, int camada, std::vector<Disciplina*> discs,
					 std::unordered_map<std::string, int>& creditos_alocados_prof);
	bool geraProfessorDisciplina(Solucao* solucao, Disciplina* disc,
								 int camada, std::unordered_map<std::string, int>& creditos_alocados_prof);
	bool geraAlocacao(Solucao* solucao, Disciplina* disc, Professor* prof, int camada);
	Solucao* gerarSolucaoAleatoria();

	std::vector<Solucao*> gerarSolucoesAleatorias(int numSolucoes);
	void reinsereGrades(Solucao* solucao) const;

	void gerarHorarioAGEfetuaCruzamento(std::vector<Solucao*>& populacao, int numCruz);
	void gerarHorarioAGEvoluiPopulacaoExper(std::vector<Solucao*>& populacao, 
										Configuracao::TipoCruzamento tipoCruz,
										std::vector<Solucao*>& pais);
	void gerarHorarioAGEfetuaMutacao(std::vector<Solucao*>& populacao);
	void gerarHorarioAGEfetuaMutacaoExper(std::vector<Solucao*>& populacao, 
									  Configuracao::TipoMutacao tipoMut);
	// Verifica se a nova população possui uma solução melhor que a anterior
	void gerarHorarioAGVerificaEvolucao(std::vector<Solucao*>& populacao, int iteracaoAtual);

	std::unique_ptr<Grade> gradeAleatoria(AlunoPerfil* alunoPerfil, Solucao* solucao);
	std::unique_ptr<Grade> vizinhoGrasp(const Grade& grade);
	void buscaLocal(std::unique_ptr<Grade>& grade);
	Grade* GRASP(AlunoPerfil* alunoPerfil, Solucao* solucao);

	std::vector<ProfessorDisciplina*> 
		getSubTour(const Solucao& pai, int xbegin, int xend) const;
	Solucao* crossoverOrdemCamada(const Solucao& pai1, const Solucao& pai2,
								  int camadaCruz);
	Solucao* crossoverOrdem(const Solucao& pai1, const Solucao& pai2);
	std::pair<int, int> getCrossoverPoints(const Solucao& pai, int camada) const;
	bool insereSubTour(const std::vector<ProfessorDisciplina*>& genes,
	                   Solucao& filho, int xbegin);
	Solucao* crossoverPMX(const Solucao& pai1, const Solucao& pai2);
	Solucao* crossoverPMXCamada(const Solucao& pai1, const Solucao& pai2, 
							    int camadaCruz);
	Solucao* crossoverCicloCamada(const Solucao& pai1, const Solucao& pai2,
								  int camadaCruz);
	Solucao* crossoverCiclo(const Solucao& pai1, const Solucao& pai2);
	Disciplina* getRandomDisc(const std::vector<Disciplina*>& restantes);

	Solucao* selecaoTorneio(const std::vector<Solucao*>& populacao) const;
	std::vector<Solucao*> populacao;
};

#endif /* RESOLUCAO_H */
