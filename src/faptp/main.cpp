#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <string>
#include <chrono>
#include <thread>

#include <boost/format.hpp>
//#include <cpr/cpr.h>

#include <faptp-lib/Resolucao.h>
#include <faptp-lib/Output.h>
#include <faptp-lib/Configuracao.h>
#include <faptp-lib/Util.h>
#include <faptp-lib/Timer.h>
#include <faptp-lib/SA.h>
#include <faptp-lib/WDJU.h>
#include <faptp-lib/HySST.h>
#include <faptp-lib/ILS.h>

/*
 * PAR�METROS DE CONFIGURA��O *
 * input.all.json ->
 *      camadasTamanho : 33
 *      perfilTamanho : 1392
 * input.json (impreciso, mas funciona) ->
 *      camadasTamaho : 4
 *      perfilTamanho : 10
 */

struct Entrada
{
  int camadasTamanho;
  int perfilTamanho;
};

constexpr Entrada input_all_json{ 33, 1392 };
constexpr Entrada input_json{ 4, 10 };

int num_tentativas_upload = 15;
int segundos_espera = 30;

// n�mero de itera��es grande para o algoritmo se encerrar por tempo
constexpr auto inf = static_cast<int>(1e9);

void
upload_result(const std::string& id, const std::string& resultado,
              int num_config, const std::string& servidor)
{
  /*for (auto i = 0; i < num_tentativas_upload; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(segundos_espera * i));

        auto r = cpr::Post(cpr::Url{servidor},
                           cpr::Payload{{"result", resultado},
                                        {"nome", id}});

        if (r.error.code == cpr::ErrorCode::OK) {
            std::cout << num_config << ") "  << id << ": enviado com
    sucesso\n\n";
            return;
        } else {
            std::cout << num_config << ") tentativa " << i+1 << " falhou: "
                << r.error.message << "\n\n";
        }
    }*/

  std::cout << num_config << ") Erro ao enviar " << id
            << ". Salvo em 'falhas/'\n";

  auto path = Util::join_path({ "falhas" });
  Util::create_folder(path);
  auto filename = path + id + ".txt";

  std::ofstream out_file{ filename };
  out_file << resultado;
}

std::string
get_auto_file_name()
{
  auto pc_name = Util::get_computer_name();
  auto pos = pc_name.find('-') + 1;
  auto pc_num = std::stoi(pc_name.substr(pos));

  auto filename = std::to_string(pc_num) + ".txt";

  return Util::join_path({ "config" }, filename);
}

void
print_violacoes(const std::unordered_map<std::string, int>& m)
{
  for (auto& p : m) {
    std::cout << p.first << " = " << p.second << "\n";
  }
  std::cout << "\n";
}

void
semArgumentos()
{
  Resolucao r{ Configuracao()
                 .arquivoEntrada(
                   Util::join_path({ "entradas" }, "input.all.json"))
                 .populacaoInicial(20)
                 .porcentagemCruzamentos(80) // %
                 .numMaximoIteracoesSemEvolucaoGRASP(15)
                 .numMaximoIteracoesSemEvolucaoAG(20)
                 .tipoCruzamento(Configuracao::TipoCruzamento::pmx)
                 .tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
                 .mutacaoProbabilidade(35) // %
                 .graspNumVizinhos(2)
                 .graspAlfa(20) // %
                 .camadaTamanho(input_all_json.camadasTamanho)
                 .perfilTamanho(input_all_json.perfilTamanho)
                 .numTorneioPopulacao(4)
                 .tentativasMutacao(4)
                 .graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
                 .tipoConstrucao(Configuracao::TipoGrade::grasp)
                 .tipoFo(Configuracao::TipoFo::Soma_carga)
                 .timeout(30000) };

  std::cout << "Montando horarios [AG + Modelo]...\n";

  auto inicio = Util::now();
  r.gerarHorarioAG();
  auto fim = Util::now();

  std::cout << "Tempo do horario: " << Util::chronoDiff(fim, inicio)
            << "ms\n\n";

  auto fo = r.getSolucao()->getFO();
  std::cout << "\nResultado:" << fo << "\n";
  // r.showResult();

  auto savePath = Util::join_path({ "teste", "fo" + std::to_string(fo) });
  Output::writeHtml(r.getSolucao(), savePath);

  std::ostringstream oss;
  oss << "solucaoAlvo: " << r.foAlvo << "\n";
  oss << "hashAlvo: " << r.hashAlvo << "\n";
  oss << "iteracoes: " << r.iteracaoAlvo << "\n";
  oss << "tempoAlvo: " << r.tempoAlvo << "\n";
  oss << "ultima iteracao: " << r.ultimaIteracao << "\n\n";
  std::cout << oss.str();

  std::ofstream out{ savePath + "log.txt" };
  // r.logExperimentos();
  out << oss.str();
  out << r.getLog();

  print_violacoes(r.getSolucao()->reportarViolacoes());
}

std::pair<long long, Solucao::FO_t>
experimento_ag(const std::string& input, int n_indiv, int taxa_mut, int p_cruz,
               const std::string& oper_cruz, int grasp_iter, int grasp_nviz,
               int grasp_alfa, int n_tour, int n_mut, long long timeout)
{
  auto cruzamento = [&] {
    if (oper_cruz == "PMX") {
      return Configuracao::TipoCruzamento::pmx;
    } else if (oper_cruz == "CX") {
      return Configuracao::TipoCruzamento::ciclo;
    } else if (oper_cruz == "OX") {
      return Configuracao::TipoCruzamento::ordem;
    } else {
      return Configuracao::TipoCruzamento::pmx;
    }
  }();

  Resolucao r{ Configuracao()
                 .arquivoEntrada(input)
                 .populacaoInicial(n_indiv)
                 .porcentagemCruzamentos(p_cruz) // %
                 .numMaximoIteracoesSemEvolucaoGRASP(grasp_iter)
                 .numMaximoIteracoesSemEvolucaoAG(inf)
                 .tipoCruzamento(cruzamento)
                 .mutacaoProbabilidade(taxa_mut) // %
                 .graspNumVizinhos(grasp_nviz)
                 .graspAlfa(grasp_alfa) // %
                 .camadaTamanho(input_all_json.camadasTamanho)
                 .perfilTamanho(input_all_json.perfilTamanho)
                 .numTorneioPopulacao(n_tour)
                 .tentativasMutacao(n_mut)
                 .timeout(timeout) };

  Timer t;
  r.gerarHorarioAG();
  auto fo = r.getSolucao()->getFO();
  auto tempo = t.elapsed();

  return { tempo, fo };
}

void
experimento_ag_cli(const std::string& input, const std::string& file,
                   const std::string& servidor, long long timeout)
{
  // coment�rio do topo
  // formato entrada:
  // ID TaxaMut NIndiv %Cruz CruzOper NMut NTour GRASPIter GRASPNVzi GRASPAlfa
  // NExec
  // formato saida:
  // ID,NExec,Tempo,Fo

  std::string conf_file;

  if (file == "auto") {
    conf_file = get_auto_file_name();
  } else {
    conf_file = file;
  }

  std::ifstream config{ conf_file };
  std::vector<std::thread> threads;

  std::string id, cruz_oper;
  int taxa_mut, n_indiv, p_cruz, n_mut, n_tour, grasp_iter;
  int grasp_nviz, grasp_alfa, n_exec;
  auto num_config = 1;

  while (config >> id >> taxa_mut >> n_indiv >> p_cruz >> cruz_oper >> n_mut >>
         n_tour >> grasp_iter >> grasp_nviz >> grasp_alfa >> n_exec) {

    auto path = Util::join_path({ "experimento" });
    Util::create_folder(path);

    auto filename = path + id + ".txt";

    std::cout << "ID: " << id << "\n\n";
    std::ostringstream out{ filename };
    out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";

    for (auto i = 0; i < n_exec; i++) {
      long long tempo;
      Solucao::FO_t fo;
      std::tie(tempo, fo) =
        experimento_ag(input, n_indiv, taxa_mut, p_cruz, cruz_oper, grasp_iter,
                       grasp_nviz, grasp_alfa, n_tour, n_mut, timeout);

      Util::logprint(out,
                     boost::format("%s,%d,%lld,%d\n") % id % i % tempo % fo);
    }
    std::cout << "\n";

    threads.emplace_back(upload_result, id, out.str(), num_config, servidor);
    num_config++;
  }
}

template <typename F>
std::string
teste_tempo_iter(int num_exec, F f)
{
  std::ostringstream oss;

  for (auto i = 0; i < num_exec; i++) {
    Resolucao r{ Configuracao()
                   .arquivoEntrada(
                     Util::join_path({ "entradas" }, "input.all.json"))
                   .populacaoInicial(20)
                   .porcentagemCruzamentos(80)
                   .numMaximoIteracoesSemEvolucaoGRASP(15)
                   .numMaximoIteracoesSemEvolucaoAG(inf)
                   .tipoCruzamento(Configuracao::TipoCruzamento::pmx)
                   .tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
                   .mutacaoProbabilidade(35) // %
                   .graspNumVizinhos(2)
                   .graspAlfa(20) // %
                   .camadaTamanho(input_all_json.camadasTamanho)
                   .perfilTamanho(input_all_json.perfilTamanho)
                   .tentativasMutacao(4)
                   .numTorneioPopulacao(4)
                   .graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
                   .tipoConstrucao(Configuracao::TipoGrade::grasp)
                   .tipoFo(Configuracao::TipoFo::Soft_constraints) };

    Util::logprint(oss, boost::format("i: %d") % (i + 1));

    Timer t;
    auto s = f(r);

    Util::logprint(oss, boost::format(" - fo: %g - t: %lld\n") % s->getFO() %
                          t.elapsed());
    Util::logprint(oss, boost::format("\t fo_alvo: %d - tempo_alvo: %lld\n") %
                          r.foAlvo % r.tempoAlvo);
    print_violacoes(s->reportarViolacoes());

    auto savePath =
      Util::join_path({ "teste", "fo" + std::to_string(s->getFO()) });
    // Output::writeHtml(s.get(), savePath);
    Output::writeJson(*s, "result.json");
  }

  Util::logprint(oss, "\n");

  return oss.str();
}

void
teste_tempo(int timeout_sec = 120)
{
  const auto timeout_ms = timeout_sec * 1000;
  const auto num_exec = 1;

  std::ostringstream oss;
  oss << std::string(25, '=') << "\n";
  oss << "Data: " << Util::date_time() << "\n";
  oss << "Tempo maximo: " << timeout_ms << "\n";
  oss << "Numero de execucoes: " << num_exec << "\n\n";

  Util::logprint(oss, "SA-ILS\n");
  oss << teste_tempo_iter(
    num_exec, [&](Resolucao& r) { return r.gerarHorarioSA_ILS(timeout_ms); });

  /*Util::logprint(oss, "HySST\n");
  oss << teste_tempo_iter(num_exec, [&](Resolucao& r) {
    return r.gerarHorarioHySST(timeout_ms, 100, 100);
  });

  Util::logprint(oss, "WDJU\n");
  oss << teste_tempo_iter(
    num_exec, [&](Resolucao& r) { return r.gerarHorarioWDJU(timeout_ms); });

  Util::logprint(oss, "AG\n");
  oss << teste_tempo_iter(num_exec, [&](Resolucao& r) {
    r.setTimeout(timeout_ms);
    return r.gerarHorarioAG()->clone();
  });*/

  std::ofstream out{ (boost::format("resultados%d.txt") % timeout_sec).str(),
                     std::ios::out | std::ios::app };
  out << oss.str() << std::endl;
}

std::pair<long long, Solucao::FO_t>
experimento_sa_ils(const std::string& input, int frac_time, double alfa,
                   double t0, int sa_iter, int sa_reaq, int sa_chances,
                   int ils_iter, int ils_pmax, int ils_p0, long long timeout)
{
  static const auto a = std::vector<std::pair<Resolucao::Vizinhanca, int>>{
    { Resolucao::Vizinhanca::ES, 25 },
    { Resolucao::Vizinhanca::EM, 43 },
    { Resolucao::Vizinhanca::RS, 20 },
    { Resolucao::Vizinhanca::RM, 10 },
    { Resolucao::Vizinhanca::KM, 2 }
  };

  static const auto b = std::vector<std::pair<Resolucao::Vizinhanca, int>>{
    { Resolucao::Vizinhanca::ES, 35 },
    { Resolucao::Vizinhanca::EM, 43 },
    { Resolucao::Vizinhanca::RS, 10 },
    { Resolucao::Vizinhanca::RM, 5 },
    { Resolucao::Vizinhanca::KM, 7 }
  };

  const auto& escolhido = [&] {
    if (sa_chances == 0) {
      return a;
    } else {
      return b;
    }
  }();

  Resolucao r{ Configuracao()
                 .arquivoEntrada(input)
                 .camadaTamanho(input_all_json.camadasTamanho)
                 .perfilTamanho(input_all_json.perfilTamanho) };

  Timer t;
  SA sa{ r, alfa, t0, sa_iter, sa_reaq, timeout / frac_time, escolhido };
  ILS ils{ r, inf, ils_pmax, ils_p0, ils_iter, timeout / frac_time };
  auto s = r.gerarHorarioSA_ILS(sa, ils, timeout);
  auto fo = s->getFO();
  auto tempo = t.elapsed();

  return { tempo, fo };
}

void
experimento_sa_ils_cli(const std::string& input, const std::string& file,
                       const std::string& servidor, long long timeout)
{
  // coment�rio do topo
  // formato entrada:
  // ID FracTime Alfa t0 SAiter SAreaq SAchances ILSiter ILSpmax ILSp0 NExec
  // formato saida:
  // ID,NExec,Tempo,Fo

  std::string conf_file;

  if (file == "auto") {
    conf_file = get_auto_file_name();
  } else {
    conf_file = file;
  }

  std::cout << "Arquivo: " << conf_file << "\n\n";

  std::ifstream config{ conf_file };
  std::vector<std::thread> threads;

  std::string id;
  int frac_time, sa_iter, sa_reaq, sa_chances, ils_iter, ils_pmax;
  int ils_p0, n_exec;
  double alfa, t0;
  auto num_config = 1;

  while (config >> id >> frac_time >> alfa >> t0 >> sa_iter >> sa_reaq >>
         sa_chances >> ils_iter >> ils_pmax >> ils_p0 >> n_exec) {

    std::cout << num_config << ") Executando ID: " << id << "\n\n";
    std::ostringstream out_str;
    out_str << "ID Algoritmo, Numero execucao, Tempo total, FO\r\n";

    for (auto i = 0; i < n_exec; i++) {
      long long tempo;
      Solucao::FO_t fo;
      std::tie(tempo, fo) =
        experimento_sa_ils(input, frac_time, alfa, t0, sa_iter, sa_reaq,
                           sa_chances, ils_iter, ils_pmax, ils_p0, timeout);

      Util::logprint(out_str,
                     boost::format("%s,%d,%lld,%d\r\n") % id % i % tempo % fo);
    }
    std::cout << "\n";

    threads.emplace_back(upload_result, id, out_str.str(), num_config,
                         servidor);
    num_config++;
  }

  for (auto& t : threads) {
    t.join();
  }
}

std::pair<long long, Solucao::FO_t>
experimento_hysst(const std::string& input, int max_level, int t_start,
                  int t_step, int it_hc, int it_mut, long long timeout)
{
  Resolucao r{ Configuracao()
                 .arquivoEntrada(input)
                 .camadaTamanho(input_all_json.camadasTamanho)
                 .perfilTamanho(input_all_json.perfilTamanho)
                 .tentativasMutacao(it_mut) };

  Timer t;
  HySST hysst{ r, timeout, 100, 100, max_level, t_start, t_step, it_hc };
  auto s = r.gerarHorarioHySST(hysst);
  auto fo = s->getFO();
  auto tempo = t.elapsed();

  return { tempo, fo };
}

void
experimento_hysst_cli(const std::string& input, const std::string& file,
                      const std::string& servidor, long long timeout)
{
  // coment�rio do topo
  // formato entrada:
  // ID MaxLevel TStart TStep IterHc IterMut NExec
  // formato saida:
  // ID,NExec,Tempo,Fo

  std::string conf_file;

  if (file == "auto") {
    conf_file = get_auto_file_name();
  } else {
    conf_file = file;
  }

  std::cout << "Arquivo: " << conf_file << "\n\n";

  std::ifstream config{ conf_file };
  std::vector<std::thread> threads;

  std::string id;
  int max_level, t_start, t_step, it_hc, it_mut, n_exec;
  auto num_config = 1;

  while (config >> id >> max_level >> t_start >> t_step >> it_hc >> it_mut >>
         n_exec) {
    auto path = Util::join_path({ "experimento" });
    Util::create_folder(path);

    auto filename = path + id + ".txt";

    std::cout << "ID: " << id << "\n\n";
    std::ostringstream out{ filename };
    out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";

    for (auto i = 0; i < n_exec; i++) {
      long long tempo;
      Solucao::FO_t fo;
      std::tie(tempo, fo) = experimento_hysst(input, max_level, t_start, t_step,
                                              it_hc, it_mut, timeout);

      Util::logprint(out,
                     boost::format("%s,%d,%lld,%d\n") % id % i % tempo % fo);
    }
    std::cout << "\n";

    threads.emplace_back(upload_result, id, out.str(), num_config, servidor);
    num_config++;
  }

  for (auto& t : threads) {
    t.join();
  }
}

std::pair<long long, Solucao::FO_t>
experimento_wdju(const std::string& input, int stag_limit, double jump_factor,
                 long long timeout)
{
  Resolucao r{ Configuracao()
                 .arquivoEntrada(input)
                 .camadaTamanho(input_all_json.camadasTamanho)
                 .perfilTamanho(input_all_json.perfilTamanho) };

  Timer t;
  WDJU wdju{ r, timeout, stag_limit, jump_factor };
  auto s = r.gerarHorarioWDJU(wdju);
  auto fo = s->getFO();
  auto tempo = t.elapsed();

  return { tempo, fo };
}

void
experimento_wdju_cli(const std::string& input, const std::string& file,
                     const std::string& servidor, long long timeout)
{
  // coment�rio do topo
  // formato entrada:
  // ID StagLimit JumpFactor NExec
  // formato saida:
  // ID,NExec,Tempo,Fo

  std::string conf_file;

  if (file == "auto") {
    conf_file = get_auto_file_name();
  } else {
    conf_file = file;
  }

  std::cout << "Arquivo: " << conf_file << "\n\n";

  std::ifstream config{ conf_file };
  std::vector<std::thread> threads;

  std::string id;
  int stag_limit, n_exec;
  double jump_factor;
  auto num_config = 1;

  while (config >> id >> stag_limit >> jump_factor >> n_exec) {
    auto path = Util::join_path({ "experimento" });
    Util::create_folder(path);

    auto filename = path + id + ".txt";

    std::cout << "ID: " << id << "\n\n";
    std::ostringstream out{ filename };
    out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";

    for (auto i = 0; i < n_exec; i++) {
      long long tempo;
      Solucao::FO_t fo;
      std::tie(tempo, fo) =
        experimento_wdju(input, stag_limit, jump_factor, timeout);

      Util::logprint(out,
                     boost::format("%s,%d,%lld,%d\n") % id % i % tempo % fo);
    }
    std::cout << "\n";

    threads.emplace_back(upload_result, id, out.str(), num_config, servidor);
    num_config++;
  }

  for (auto& t : threads) {
    t.join();
  }
}

int
main(int argc, char* argv[])
{
  static const char usage[] = R"(
USAGE:

    faptp-lib -h
    faptp-lib <algo> <entrada> <configuracao> <servidor>

Onde:

    -h, --help
        Mostra essa mensagem de ajuda e sai.

    <algo>
        Algoritmo a ser executado. Deve ser um entre: -ag, -sa_ils, -hysst, -wdju.

    <entrada>
        Arquivo de entrada.

    <configuracao>
        Arquivo de configura��o. Se for 'auto', ir� pegar o n�mero do arquivo
        de acordo com o n�mero da m�quina.

    <servidor>
        Endere�o IP do servidor para onde os resultados ser�o enviados.
)";

  const auto timeout = 60000;

  if (argc == 5) {
    std::string algo = argv[1];
    // Primeiro argumento � o algoritmo {-ag, -sa_ils, -hysst, -wdju}
    // segundo � o arquivo de entrada, terceiro � o de configura��o
    if (algo == "-ag") {
      experimento_ag_cli(argv[2], argv[3], argv[4], timeout);
    } else if (algo == "-sa_ils") {
      experimento_sa_ils_cli(argv[2], argv[3], argv[4], timeout);
    } else if (algo == "-hysst") {
      experimento_hysst_cli(argv[2], argv[3], argv[4], timeout);
    } else if (algo == "-wdju") {
      experimento_wdju_cli(argv[2], argv[3], argv[4], timeout);
    } else {
      std::cout << "Algoritmo invalido\n";
    }
  } else if (argc == 2) {
    std::string flag = argv[1];
    if (flag == "-h" || flag == "--help") {
      std::cout << usage << "\n";
    }
  } else {
    // semArgumentos();

    teste_tempo(1 * 60);
    //teste_tempo(3 * 60);
  }
}
