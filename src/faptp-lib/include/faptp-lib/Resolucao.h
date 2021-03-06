#ifndef RESOLUCAO_H
#define RESOLUCAO_H

#include <memory>
#include <chrono>
#include <vector>
#include <tsl/robin_map.h>
#include <tsl/hopscotch_set.h>
#include <string>
#include <sstream>

#ifdef MODELO
#include <modelo-grade/aluno.h>
#include <modelo-grade/curso.h>
#endif

#include <jsoncpp/json/json.h>
#include <faptp-lib/Configuracao.h>

class Professor;

class Disciplina;
class ProfessorDisciplina;
class AlunoPerfil;
class Solucao;
class Grade;

class WDJU;
class ILS;
class SA;
class HySST;

class Resolucao {
public:

  enum class Vizinhanca {
    ES,
    EM,
    RS,
    RM,
    PR,
    KM,
    HC_FI,
    HC_EC
  };

  Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho,
            Configuracao::TipoGrade pTipoConstrucao,
            std::string arquivoEntrada = "input.json");
  Resolucao(const Configuracao& c);
  virtual ~Resolucao();

  double start();
  double start(bool input);
  long long timeout() const;
  int numThreadsAG() const;
  int numThreadsGRASP() const;
  int numThreadsProdutorConsumidor() const;
  double getGradeAlfa() const;

  int getBlocosTamanho() const;
  const std::map<std::string, std::vector<Disciplina*>>&
      getPeriodoXDisciplinas() const;
  int getCamadasTamanho() const;
  void setTimeout(long timeout);
  void setNumThreadsAG(int numThreads);
  void setNumThreadsGRASP(int numThreads);
  void setNumParesProdutorConsumidor(int numPares);
  void setGradeAlfa(int alfa);

  Solucao* gerarHorarioAG();
  Solucao* gerarHorarioAGSerial();
  Solucao* gerarHorarioAGPar();
  Solucao* gerarHorarioAGMultiPopulacao();
  Solucao* gerarHorarioAGMultiPopulacaoOpenMP();

  std::unique_ptr<Solucao> gerarHorarioSA_ILS(long long timeout);
  std::unique_ptr<Solucao> gerarHorarioSA_ILS(SA& sa, ILS& ils, long long timeout);

  std::unique_ptr<Solucao> gerarHorarioWDJU(long long timeout);
  std::unique_ptr<Solucao> gerarHorarioWDJU(WDJU& wdju);

  std::unique_ptr<Solucao> gerarHorarioHySST(
    long long tempo_total, long long tempo_mu, long long tempo_hc);
  std::unique_ptr<Solucao> gerarHorarioHySST(HySST& HySST, int it_mut);

  std::unique_ptr<Solucao> carregarSolucao(const std::string& file);

  void gerarGrade() const;
  void gerarGrade(Solucao* pSolucao) const;

  void showResult();

  Solucao* getSolucao();

  void teste();

#ifdef MODELO
  fagoc::Curso& getCurso();
  const std::vector<fagoc::Aluno>& getAlunos() const;
#endif
  std::string getLog() const;

  // Converte a matriz tridimensional do hor�rio em uma matriz bidimensional
  // de bin�rios para o modelo
  std::vector<std::vector<char>> converteHorario(Solucao* pSolucao) const;

  // Operadores de vizinhan�a
  std::unique_ptr<Solucao> event_swap(const Solucao& sol) const;
  std::unique_ptr<Solucao> event_move(const Solucao& sol) const;
  std::unique_ptr<Solucao> resource_swap(const Solucao& sol) const;
  std::unique_ptr<Solucao> resource_move(const Solucao& sol) const;
  std::unique_ptr<Solucao> permute_resources(const Solucao& sol) const;
  std::unique_ptr<Solucao> kempe_move(const Solucao& sol) const;

  const tsl::robin_map<std::string, Professor*>& getProfessores() const;
  const std::vector<Disciplina*>& getDisciplinas() const;

  std::unique_ptr<Grade> gradeAleatoria(AlunoPerfil* alunoPerfil,
                                        const Solucao* solucao) const;
  void buscaLocal(std::unique_ptr<Grade>& grade) const;

  Disciplina* getDisciplinaById(const std::string& disc_id);

  Disciplina* getDisciplinaByCode(std::size_t d_code);
  const Disciplina& getDisciplinaByCode(std::size_t d_code) const;

  /*
       Par�metros da execu��o da solu��o
       */
  // Hor�rio popula��o inicial
  int horarioPopulacaoInicial;
  int horarioProfessorColisaoMax = 2;

  // Hor�rio torneio
  double horarioTorneioPares;
  double horarioTorneioPopulacao;

  // Hor�rio cruzamento
  double horarioCruzamentoPorcentagem = 0;
  int horarioCruzamentoFilhos = 2;
  int horarioCruzamentoDias{};
  double horarioCruzamentoCamadas{};
  int horarioCruzamentoTentativasMax = 1;
  // Operador de cruzamento selecionado
  Configuracao::TipoCruzamento horarioTipoCruzamento;
  // Operador de muta��o selecionado
  Configuracao::TipoMutacao horarioTipoMutacao;
  // FO utilizada
  Configuracao::TipoFo horarioTipoFo;
  // Versão do AG
  Configuracao::Versao_AG versaoAg;
  // Versão do GRASP
  Configuracao::Versao_GRASP versaoGrasp;

  // Mapa dos pesos das restri��es
  tsl::robin_map<std::string, double> pesos_soft;

  int horarioIteracao;

  // Hor�rio muta��o
  double horarioMutacaoProbabilidade;
  int horarioMutacaoTentativas;

  // Grade tipo de constru��o
  Configuracao::TipoGrade gradeTipoConstrucao;

  // Grade GRASP
  Configuracao::TipoVizinhos gradeGraspVizinhanca;
  int gradeGraspVizinhos;
  double gradeGraspTempoConstrucao;

  // Solu��o alvo, a itera��o em que ela foi alcan�ada primeiro e o tempo
  double foAlvo{};
  int iteracaoAlvo{};
  long long tempoAlvo{};
  std::size_t hashAlvo{};

  void logExperimentos();

  // Porcentagem de solu��es aleat�rias que ser�o criadas e adicionadas � popula��o
  // � cada itera��o
  double porcentagemSolucoesAleatorias;

  // N�mero de itera��es m�ximo que o AG continuar� sem evoluir a solu��o
  int maxIterSemEvolAG;
  int ultimaIteracao;
  // N�mero de itera��es m�ximo que o GRASP continuar� sem evoluir a grade
  int maxIterSemEvoGrasp;

  static const int numcruz = 6;
  int contadorIguaisCruz[numcruz] {};
  int contadorIguaisMut[numcruz] {};
  int contadorMelhoresCruz[numcruz] {};
  int contadorMelhoresMut[numcruz] {};
  long long tempoTotalCruz[numcruz] {};
  long long tempoTotalMut[numcruz] {};
  long long tempoTotalSelec{};
  long long tempoTotalElit{};

  std::vector<Solucao*> gerarHorarioAGTorneioPar(std::vector<Solucao*>& solucoesPopulacao) const;
  std::vector<Solucao*> gerarHorarioAGCruzamento(const std::vector<Solucao*>& parVencedor) const;
  Solucao* gerarHorarioAGMutacao(const Solucao* pSolucao) const;
private:
  int blocosTamanho;
  int camadasTamanho;
  int perfisTamanho;
  std::string arquivoEntrada;
  tsl::robin_map<std::string, Professor*> professores;
  std::vector<Disciplina*> disciplinas;
  tsl::robin_map<std::string, int> disciplinasIndex;
  std::map<std::string, std::vector<Disciplina*>> periodoXdisciplina;
  tsl::robin_map<std::string, AlunoPerfil*> alunoPerfis;
  mutable tsl::robin_map<std::string, ProfessorDisciplina*> professorDisciplinas;
  Solucao* solucao;
  Json::Value jsonRoot;
#ifdef MODELO
  std::unique_ptr<fagoc::Curso> curso;
  std::vector<fagoc::Aluno> alunos;
  double tempoLimiteModelo;
#endif
  std::chrono::high_resolution_clock::time_point tempoInicio;
  std::ostringstream log;
  long long timeout_;
  int numThreadsAG_;
  int numThreadsGRASP_;
  int numParesProdutorConsumidor_;
  std::vector<Solucao*> populacao;
  double gradeAlfa;
  // 1 - gradeAlfa
  double gradeAlfaCompl;

  void carregarDados();
  void carregarDadosProfessores();
  void carregarDadosDisciplinas();
  void carregarAlunoPerfis();
  void carregarDadosProfessorDisciplinas();
  void carregarSolucaoOld();
  std::unique_ptr<Solucao> carregarSolucao(const Json::Value& horarios);

  void atualizarDisciplinasIndex();
  std::vector<Solucao*> gerarHorarioAGCruzamentoExper(const std::vector<Solucao*>& parVencedor,
      Configuracao::TipoCruzamento tipoCruz);
  Solucao* gerarHorarioAGTorneio(std::vector<Solucao*>& solucoesPopulacao) const;
  Solucao* gerarHorarioAGTorneio2(std::vector<Solucao*>& pop) const;

  std::vector<Solucao*> gerarHorarioAGCruzamentoConstrutivoReparo(const Solucao* solucaoPai1, const Solucao* solucaoPai2);
  bool gerarHorarioAGCruzamentoAleatorioReparoBloco(Solucao*& solucaoFilho, int diaG, int blocoG, int camadaG);
  bool gerarHorarioAGCruzamentoAleatorioReparo(Solucao*& solucaoFilho, int diaG, int blocoG, int camadaG);

  int cruzaCamada(Solucao*& filho, const Solucao* pai, int camada) const;
  std::vector<Solucao*> gerarHorarioAGCruzamentoSimples(Solucao* pai1, Solucao* pai2);

  std::vector<Solucao*> gerarHorarioAGCruzamentoSubstBloco(Solucao* solucaoPai1, Solucao* solucaoPai2);

  void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*>& pop);
  void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*>& pop, int populacaoMax) const;
  std::vector<Solucao*> gerarHorarioAGMutacao(const std::vector<Solucao*>& pop) const;
  std::vector<Solucao*> gerarHorarioAGMutacaoExper(std::vector<Solucao*>& pop,
      Configuracao::TipoMutacao tipoMut);
  Solucao* gerarHorarioAGMutacaoSubstDisc(Solucao* pSolucao);
  bool swapSlots(Solucao& sol, int posX1, int posX2) const;

  void gerarGradeTipoGrasp(Solucao* sol) const;

  double gerarGradeTipoGuloso(Solucao*& pSolucao);

  Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, int maxDeep, int deep,
      std::vector<std::size_t>::const_iterator current);
  Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, int maxDeep);
  double gerarGradeTipoCombinacaoConstrutiva(Solucao*& pSolucao);

  void gerarGradeTipoGraspConstrucao(Solucao* pSolucao);

  void gerarGradeTipoGraspConstrucao(Grade* pGrade);
  void gerarGradeTipoGraspConstrucao(Grade* pGrade, std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar);
  Solucao* gerarGradeTipoGraspRefinamentoAleatorio(Solucao* pSolucao);
  Solucao* gerarGradeTipoGraspRefinamentoCrescente(Solucao* pSolucao);

  double gerarGradeTipoModelo(Solucao* pSolucao);

  int getIntervaloAlfaGrasp(const std::vector<Disciplina*>& restantes) const;

  void showResult(Solucao* pSolucao);

  Solucao* gerarHorarioAGMutacaoSubstProf(const Solucao& pSolucao) const;

  void logPopulacao(const std::vector<Solucao*>& pop, int iter);

  std::vector<Solucao*> gerarHorarioAGPopulacaoInicialParalelo();
  std::vector<Solucao*> gerarHorarioAGPopulacaoInicialSerial();
  std::vector<Solucao*> gerarHorarioAGPopulacaoInicialSerial(int num_individuos);

  bool gerarCamada(Solucao* sol, int camada, const std::vector<Disciplina*>& discs,
                   tsl::robin_map<std::string, int>& creditos_alocados_prof) const;
  bool geraProfessorDisciplina(Solucao* sol, Disciplina* disc,
                               int camada, tsl::robin_map<std::string, int>& creditos_alocados_prof) const;
  bool geraAlocacao(Solucao* sol, Disciplina* disc, Professor* prof, int camada) const;
  std::unique_ptr<Solucao> gerarSolucaoAleatoria() const;
  std::unique_ptr<Solucao> gerarSolucaoAleatoriaNotNull() const;

  std::vector<Solucao*> gerarSolucoesAleatoriasParalelo(int numSolucoes);
  std::vector<Solucao*> gerarSolucoesAleatoriasSerial(int numSolucoes);

  void reinsereGrades(Solucao* sol) const;

  void gerarHorarioAGEfetuaCruzamento(std::vector<Solucao*>& pop, int numCruz);
  void gerarHorarioAGEvoluiPopulacaoExper(std::vector<Solucao*>& pop,
                                          Configuracao::TipoCruzamento tipoCruz,
                                          std::vector<Solucao*>& pais);
  void gerarHorarioAGEfetuaMutacao(std::vector<Solucao*>& pop) const;
  void gerarHorarioAGEfetuaMutacaoExper(std::vector<Solucao*>& pop,
                                        Configuracao::TipoMutacao tipoMut);
  // Verifica se a nova popula��o possui uma solu��o melhor que a anterior
  void gerarHorarioAGVerificaEvolucao(std::vector<Solucao*>& pop, int iteracaoAtual);

  std::unique_ptr<Grade> vizinhoGrasp(const Grade& grade) const;
  Grade* GRASP(AlunoPerfil* alunoPerfil, Solucao* solucao) const;
  Grade* GRASP_serial(AlunoPerfil* alunoPerfil, Solucao* solucao) const;
  Grade* GRASP_par_1(AlunoPerfil* alunoPerfil, Solucao* solucao) const;
  Grade* GRASP_par_2(AlunoPerfil* alunoPerfil, Solucao* solucao) const;
  Grade* GRASP_par_3(AlunoPerfil* alunoPerfil, Solucao* solucao) const;


  /*
   * Helpers dos movimentos de resource
  */

  // Remove as inst�ncias de `aloc` na camada `camada` da solu��o `sol`,
  // salvando os lugares em pares (dia, bloco)
  std::vector<std::pair<int, int>> remove_aloc_memorizando(
      Solucao& sol, const ProfessorDisciplina* aloc, int camada) const;

  // Reinsere um ProfessorDisciplina `aloc` na camada `camada` da solu��o
  // `sol`, nas posi��es registradas por `posicoes_aloc`
  bool reinsere_alocacoes(
    Solucao& sol, const std::vector<std::pair<int, int>>& posicoes_aloc,
    ProfessorDisciplina* aloc, int camada) const;

  // Retorna uma par (posi��o, aloca��o), onde a posi��o � gerada aleatoriamente,
  // a aloca��o � aquela presente nessa posi��o na matriz da solu��o `sol`
  // e n�o � nula
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

  bool swap_blocos(Solucao& sol, const std::tuple<int, int>& t1,
                   const std::tuple<int, int>& t2, int camada) const;

  std::vector<ProfessorDisciplina*>
  getSubTour(const Solucao& pai, int xbegin, int xend) const;
  Solucao* crossoverOrdemCamada(const Solucao& pai1, const Solucao& pai2,
                                int camadaCruz) const;
  Solucao* crossoverOrdem(const Solucao& pai1, const Solucao& pai2) const;
  std::pair<int, int> getCrossoverPoints(const Solucao& pai, int camada) const;
  bool insereSubTour(const std::vector<ProfessorDisciplina*>& genes,
                     Solucao& filho, int xbegin) const;
  Solucao* crossoverPMX(const Solucao& pai1, const Solucao& pai2) const;
  Solucao* crossoverPMXCamada(const Solucao& pai1, const Solucao& pai2,
                              int camadaCruz) const;
  std::tuple<std::vector<std::string>, std::vector<int>, std::vector<int>>
      crossoverPMXCriarRepr(const Solucao& pai1, const Solucao& pai2, int camada) const;
  std::vector<std::string> inverterPMXRepr(
    const tsl::robin_map<std::string, std::vector<int>>& mapping) const;
  std::vector<int> crossoverPMXSwap(const std::vector<int>& pai1,
                                    const std::vector<int>& pai2,
                                    int xbegin, int xend) const;
  Solucao* crossoverCicloCamada(const Solucao& pai1, const Solucao& pai2,
                                int camadaCruz) const;
  Solucao* crossoverCiclo(const Solucao& pai1, const Solucao& pai2) const;
  Disciplina* getRandomDisc(const std::vector<Disciplina*>& restantes) const;

  Solucao* selecaoTorneio(const std::vector<Solucao*>& pop) const;
  void printCamada(const Solucao& s, int camada) const;

  static void resetIds();
};

#endif /* RESOLUCAO_H */

