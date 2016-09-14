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

// forwardar declaration dos algoritmos: dependência circular entre eles
// a Resolução (bad design, eu sei)
class WDJU;
class ILS;
class SA;

class Resolucao
{
public:

    enum class Vizinhanca
    {
        ES,
        EM,
        RS,
        RM,
        PR,
        KM
    };

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

    std::unique_ptr<Solucao> gerarHorarioSA_ILS(long long timeout);
    std::unique_ptr<Solucao> gerarHorarioSA_ILS(const SA& sa, const ILS& ils);

    std::unique_ptr<Solucao> gerarHorarioWDJU(long long timeout);
    std::unique_ptr<Solucao> gerarHorarioWDJU(const WDJU& wdju);

    double gerarGrade() const;

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
    std::vector<std::vector<char>> converteHorario(Solucao* pSolucao) const;

    // Operadores de vizinhança
    std::unique_ptr<Solucao> event_swap(const Solucao& sol) const;
    std::unique_ptr<Solucao> event_move(const Solucao& sol) const;
    std::unique_ptr<Solucao> resource_swap(const Solucao& sol) const;
    std::unique_ptr<Solucao> resource_move(const Solucao& sol) const;
    std::unique_ptr<Solucao> permute_resources(const Solucao& sol) const;
    std::unique_ptr<Solucao> kempe_move(const Solucao& sol) const;

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
    double tempoLimiteModelo;
#endif
    std::chrono::high_resolution_clock::time_point tempoInicio;
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
    Solucao* gerarHorarioAGTorneio2(std::vector<Solucao*>& pop) const;

    std::vector<Solucao*> gerarHorarioAGCruzamentoConstrutivoReparo(Solucao* solucaoPai1, Solucao* solucaoPai2);
    bool gerarHorarioAGCruzamentoAleatorioReparoBloco(Solucao*& solucaoFilho, int diaG, int blocoG, int camadaG);
    bool gerarHorarioAGCruzamentoAleatorioReparo(Solucao*& solucaoFilho, int diaG, int blocoG, int camadaG);

    int cruzaCamada(Solucao*& filho, const Solucao* pai, int camada) const;
    std::vector<Solucao*> gerarHorarioAGCruzamentoSimples(Solucao* pai1, Solucao* pai2);

    std::vector<Solucao*> gerarHorarioAGCruzamentoSubstBloco(Solucao* solucaoPai1, Solucao* solucaoPai2);

    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*>& pop);
    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*>& pop, int populacaoMax) const;
    std::vector<Solucao*> gerarHorarioAGMutacao(std::vector<Solucao*>& pop);
    std::vector<Solucao*> gerarHorarioAGMutacaoExper(std::vector<Solucao*>& pop,
                                                     Configuracao::TipoMutacao tipoMut);
    Solucao* gerarHorarioAGMutacaoSubstDisc(Solucao* pSolucao);
    bool swapSlots(Solucao& sol, int posX1, int posX2) const;
    Solucao* gerarHorarioAGMutacao(Solucao* pSolucao);

    double gerarGradeTipoGrasp2(Solucao* sol) const;
    double gerarGrade(Solucao* pSolucao) const;

    double gerarGradeTipoGuloso(Solucao*& pSolucao);

    Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, int maxDeep, int deep,
                                               std::unordered_set<std::string>::const_iterator current);
    Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, int maxDeep);
    double gerarGradeTipoCombinacaoConstrutiva(Solucao*& pSolucao);

    void gerarGradeTipoGraspConstrucao(Solucao* pSolucao);

    void gerarGradeTipoGraspConstrucao(Grade* pGrade);
    void gerarGradeTipoGraspConstrucao(Grade* pGrade, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
    Solucao* gerarGradeTipoGraspRefinamentoAleatorio(Solucao* pSolucao);
    Solucao* gerarGradeTipoGraspRefinamentoCrescente(Solucao* pSolucao);
    [[deprecated("Use gerarGradeTipoGrasp2")]]
    double gerarGradeTipoGrasp();
    [[deprecated("Use gerarGradeTipoGrasp2")]]
    double gerarGradeTipoGrasp(Solucao*& pSolucao);

    double gerarGradeTipoModelo(Solucao* pSolucao);

    int getIntervaloAlfaGrasp(const std::vector<Disciplina*>& apRestante) const;

    void showResult(Solucao* pSolucao);

    Solucao* gerarHorarioAGMutacaoSubstProf(const Solucao& pSolucao) const;

    void logPopulacao(const std::vector<Solucao*>& pop, int iter);

    std::vector<Solucao*> gerarHorarioAGPopulacaoInicial2();
    bool gerarCamada(Solucao* sol, int camada, std::vector<Disciplina*> discs,
                     std::unordered_map<std::string, int>& creditos_alocados_prof);
    bool geraProfessorDisciplina(Solucao* sol, Disciplina* disc,
                                 int camada, std::unordered_map<std::string, int>& creditos_alocados_prof);
    bool geraAlocacao(Solucao* sol, Disciplina* disc, Professor* prof, int camada);
    std::unique_ptr<Solucao> gerarSolucaoAleatoria();
    std::unique_ptr<Solucao> gerarSolucaoAleatoriaNotNull();

    std::vector<Solucao*> gerarSolucoesAleatorias(int numSolucoes);
    std::vector<Solucao*> gerarSolucoesAleatorias2(int numSolucoes);
    void reinsereGrades(Solucao* sol) const;

    void gerarHorarioAGEfetuaCruzamento(std::vector<Solucao*>& pop, int numCruz);
    void gerarHorarioAGEvoluiPopulacaoExper(std::vector<Solucao*>& pop,
                                            Configuracao::TipoCruzamento tipoCruz,
                                            std::vector<Solucao*>& pais);
    void gerarHorarioAGEfetuaMutacao(std::vector<Solucao*>& pop);
    void gerarHorarioAGEfetuaMutacaoExper(std::vector<Solucao*>& pop,
                                          Configuracao::TipoMutacao tipoMut);
    // Verifica se a nova população possui uma solução melhor que a anterior
    void gerarHorarioAGVerificaEvolucao(std::vector<Solucao*>& pop, int iteracaoAtual);

    std::unique_ptr<Grade> gradeAleatoria(AlunoPerfil* alunoPerfil, 
                                          Solucao* solucao) const;
    std::unique_ptr<Grade> vizinhoGrasp(const Grade& grade) const;
    void buscaLocal(std::unique_ptr<Grade>& grade) const;
    Grade* GRASP(AlunoPerfil* alunoPerfil, Solucao* solucao) const;
;

    /*
     * Helpers dos movimentos de resource
    */

    // Remove as instâncias de `aloc` na camada `camada` da solução `sol`,
    // salvando os lugares em pares (dia, bloco)
    std::vector<std::pair<int, int>> remove_aloc_memorizando(
        Solucao& sol, ProfessorDisciplina* aloc, int camada) const;

    // Reinsere um ProfessorDisciplina `aloc` na camada `camada` da solução
    // `sol`, nas posições registradas por `posicoes_aloc`
    bool reinsere_alocacoes(
        Solucao& sol, const std::vector<std::pair<int, int>>& posicoes_aloc,
        ProfessorDisciplina* aloc, int camada) const;

    // Retorna uma par (posição, alocação), onde a posição é gerada aleatoriamente,
    // a alocação é aquela presente nessa posição na matriz da solução `sol` 
    // e não é nula
    std::pair<int, ProfessorDisciplina*> get_random_notnull_aloc(
        const Solucao& sol) const;

    // Verifica se o professor `prof` consta na lista de habilitados de `disc`
    bool is_professor_habilitado(const Disciplina& disc, Professor* prof) const;

    // Encontra um sub-grafo conexo em um grafo
    std::vector<std::vector<int>> encontra_subgrafos_conexos(
        const std::vector<std::vector<int>>& grafo
    ) const;

    std::vector<int> dfs(
        const std::vector<std::vector<int>>& grafo,
        int no_inicial,
        std::vector<bool>& visitados
    ) const;

    std::unique_ptr<Solucao> swap_timeslots(
        const Solucao& sol,
        const std::tuple<int, int>& t1,
        const std::tuple<int, int>& t2,
        const std::vector<int>& cadeia
    ) const;

    bool swap_blocos(Solucao& sol, const std::tuple<int, int> & t1, 
                     const std::tuple<int, int> & t2, int camada) const;

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
                                  int camadaCruz) const;
    Solucao* crossoverCiclo(const Solucao& pai1, const Solucao& pai2) const;
    Disciplina* getRandomDisc(const std::vector<Disciplina*>& restantes) const;

    Solucao* selecaoTorneio(const std::vector<Solucao*>& pop) const;
    std::vector<Solucao*> populacao;
};

#endif /* RESOLUCAO_H */

