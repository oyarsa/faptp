#include <faptp-lib/Configuracao.h>
#include <faptp-lib/HySST.h>
#include <faptp-lib/ILS.h>
#include <faptp-lib/Output.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/SA.h>
#include <faptp-lib/Constantes.h>
#include <faptp-lib/WDJU.h>

#include <faptp-lib/DadosModelo.h>
#include <faptp-lib/Modelo.h>

#include <fstream>
#include <iostream>
#include <memory>

#include <json/json.h>
#include <fmt/format.h>
#include <cxxopts.hpp>

constexpr auto infinito = static_cast<int>(1e9);

std::unique_ptr<Solucao> wdju(Resolucao& r, const Json::Value& json)
{
  const auto stag_limit = json["StagLimit"].asInt();
  const auto jump_factor = json["JumpFactor"].asDouble();
  WDJU wdju{ r, r.timeout(), stag_limit, jump_factor };
  return r.gerarHorarioWDJU(wdju);
}

std::unique_ptr<Solucao> ag(Resolucao& r, const Json::Value& json)
{
  r.horarioPopulacaoInicial = json["NIndiv"].asInt();
  r.horarioCruzamentoPorcentagem = json["%Cruz"].asInt() / 100.0;
  r.horarioMutacaoTentativas = json["NMut"].asInt();
  r.horarioTorneioPopulacao = json["NTour"].asInt();
  r.maxIterSemEvolAG = json["AGIter"].asInt();
  r.horarioTipoCruzamento = [&json]() {
    const auto x = json["CruzOper"].asString();
    if (x == "CX") return Configuracao::TipoCruzamento::ciclo;
    else if (x == "OX") return Configuracao::TipoCruzamento::ordem;
    else /* PMX */ return Configuracao::TipoCruzamento::pmx; 
  }();
  r.horarioMutacaoProbabilidade = json["TaxaMut"].asInt() / 100.0;
  r.versaoAg = [&json]() {
    const auto x = json.get("VersaoAG", "Serial").asString();
    if (x == "Paralelo")
      return Configuracao::Versao_AG::Paralelo;
    else if (x == "Single")
      return Configuracao::Versao_AG::Serial;
    else {
      fmt::print("Versao do AG invalida");
      std::exit(1);
    }
  }();
  r.setNumThreadsAG(json.get("NumThreadsAG", 1).asInt());

  auto sol = r.gerarHorarioAG()->clone();

  return sol;
}

std::unique_ptr<Solucao> sails(Resolucao& r, const Json::Value& json)
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

  const auto sa_chances = json["SAchance"].asInt();
  const auto alfa = json["SAlfa"].asDouble();
  const auto t0 = json["t0"].asDouble();
  const auto sa_iter = json["SAiter"].asInt();
  const auto sa_reaq = json["SAreaq"].asInt();
  const auto frac_time = json["FracTime"].asInt();
  const auto ils_p0 = json["ILSp0"].asInt();
  const auto ils_pmax = json["ILSpmax"].asInt();
  const auto ils_iter = json["ILSiter"].asInt();

  const auto& escolhido = [&] {
    if (sa_chances == 0) {
      return a;
    } else {
      return b;
    }
  }();

  SA sa{ r, alfa, t0, sa_iter, sa_reaq, r.timeout() / frac_time, escolhido };
  ILS ils{ r, infinito, ils_pmax, ils_p0, ils_iter, r.timeout() / frac_time };
  return r.gerarHorarioSA_ILS(sa, ils, r.timeout());
}

std::unique_ptr<Solucao> hysst(Resolucao& r, const Json::Value& json)
{
  const auto it_mut = json["IterMut"].asInt();
  const auto max_level = json["MaxLevel"].asInt();
  const auto t_start = json["TStart"].asInt();
  const auto t_step = json["TStep"].asInt();
  const auto it_hc = json["IterHc"].asInt();

  r.horarioMutacaoTentativas = it_mut;
  HySST hysst{ r, r.timeout(), 100, 100, max_level, t_start, t_step, it_hc };

  return r.gerarHorarioHySST(hysst, it_mut);
}

void run(const std::string& conf, const std::string& input, 
         const std::string& out, const std::string& mode)
{
  Json::Value json;
  {
    std::ifstream file{ conf };
    file >> json;
  }

  dias_semana_util = json["numeroDiasLetivos"].asInt();
  const auto numeroHorarios = json["numeroHorarios"].asInt();
  const auto numeroAlunos = json["numeroAlunos"].asInt();
  const auto numeroPeriodos = json["numeroPeriodos"].asInt();
  const auto timeout = json["tempo"].asInt();
  const auto fo = [&json]() {
    auto x = json["fo"].asString();
    if (x == "pref") return Configuracao::TipoFo::Soft_constraints;
    else return Configuracao::TipoFo::Soma_carga;
  }();

  Resolucao r{ Configuracao()
    .arquivoEntrada(input)
    .blocoTamanho(numeroHorarios)
    .camadaTamanho(numeroPeriodos)
    .perfilTamanho(numeroAlunos)
    .timeout(timeout * 1000)
    .tipoFo(fo) };

  r.gradeAlfa = json["parametros"]["GAlfa"].asInt();
  r.maxIterSemEvoGrasp = json["parametros"]["GIter"].asInt();
  r.gradeGraspVizinhos = json["parametros"]["GNViz"].asInt();
  r.versaoGrasp = [&json]() {
    const auto x = json["parametros"].get("VersaoGRASP", "Serial");
    if (x == "Serial")
      return Configuracao::Versao_GRASP::Serial;
    else if (x == "Paralelo")
      return Configuracao::Versao_GRASP::Paralelo;
    else if (x == "ProdutorConsumidor")
      return Configuracao::Versao_GRASP::ProdutorConsumidor;
    else
      return Configuracao::Versao_GRASP::Paralelo_ProdutoConsumidor;
  }();
  r.setNumThreadsGRASP(json["parametros"].get("NumThreadsGRASP", 1).asInt());
  r.setNumParesProdutorConsumidor(
      json["parametros"].get("NumParesProdutorConsumidor", 1).asInt());

  auto restricoes = {
    "Janelas", "IntervalosTrabalho", "NumDiasAula", "AulasSabado", 
    "AulasSeguidas", "AulasSeguidasDificil", "AulaDificilUltimoHorario",
    "PreferenciasProfessores", "AulasProfessores"
  };

  for (auto k : restricoes)
    r.pesos_soft[k] = json["pesos"][k].asDouble();

  auto algoritmo = json["algoritmo"].asString();

  auto solucao = [&]() {
    if (mode == "grade") return r.carregarSolucao(input);
    else if (algoritmo == "AG") return ag(r, json["parametros"]);
    else if (algoritmo == "HySST") return  hysst(r, json["parametros"]);
    else if (algoritmo == "SA-ILS") return sails(r, json["parametros"]);
    else /* WDJU */ return wdju(r, json["parametros"]);
  }();
  
  if (!solucao) {
    Output::writeError("Solu��o infact�vel", out);
    return;
  }

  r.gerarGrade(solucao.get());
  Output::writeJson(*solucao, out);
}

template<typename Number>
Number average(const std::vector<Number>& timings)
{
  return std::accumulate(begin(timings), end(timings), Number{0}) / timings.size();
}

template <typename Number>
Number median(std::vector<Number> v)
{
  std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
  return v[v.size() / 2];
}

void run_many(const std::string& conf, const std::string& input,
              [[maybe_unused]] const std::string&out,
              const std::string& mode,
              const int num_repetitions)
{
  Json::Value json;
  {
    std::ifstream file{ conf };
    file >> json;
  }

  dias_semana_util = json["numeroDiasLetivos"].asInt();
  const auto numero_horarios = json["numeroHorarios"].asInt();
  const auto numero_alunos = json["numeroAlunos"].asInt();
  const auto numero_periodos = json["numeroPeriodos"].asInt();
  const auto timeout = json["tempo"].asInt();
  const auto fo = [&json]() {
    auto x = json["fo"].asString();
    if (x == "pref") return Configuracao::TipoFo::Soft_constraints;
    else return Configuracao::TipoFo::Soma_carga;
  }();

  Resolucao r{ Configuracao()
    .arquivoEntrada(input)
    .blocoTamanho(numero_horarios)
    .camadaTamanho(numero_periodos)
    .perfilTamanho(numero_alunos)
    .timeout(timeout * 1000)
    .tipoFo(fo) };

  r.gradeAlfa = json["parametros"]["GAlfa"].asInt();
  r.maxIterSemEvoGrasp = json["parametros"]["GIter"].asInt();
  r.gradeGraspVizinhos = json["parametros"]["GNViz"].asInt();
  r.versaoGrasp = [&json]() {
    const auto x = json["parametros"].get("VersaoGRASP", "Serial");
    if (x == "Single")
      return Configuracao::Versao_GRASP::Serial;
    else if (x == "Paralelo")
      return Configuracao::Versao_GRASP::Paralelo;
    else if (x == "ProdutorConsumidor")
      return Configuracao::Versao_GRASP::ProdutorConsumidor;
    else if (x == "Paralelo_ProdutorConsumidor")
      return Configuracao::Versao_GRASP::Paralelo_ProdutoConsumidor;
    else {
      fmt::print("Versao do GRASP invalida\n");
      std::exit(1);
    }
  }();
  r.setNumThreadsGRASP(json["parametros"].get("NumThreadsGRASP", 1).asInt());
  r.setNumParesProdutorConsumidor(
      json["parametros"].get("NumParesProdutorConsumidor", 1).asInt());

  auto restricoes = {
    "Janelas", "IntervalosTrabalho", "NumDiasAula", "AulasSabado",
    "AulasSeguidas", "AulasSeguidasDificil", "AulaDificilUltimoHorario",
    "PreferenciasProfessores", "AulasProfessores"
  };

  for (auto k : restricoes)
    r.pesos_soft[k] = json["pesos"][k].asDouble();

  auto algoritmo = json["algoritmo"].asString();

  const auto run_algorithm = [&]() {
    if (mode == "grade") return r.carregarSolucao(input);
    else if (algoritmo == "AG") return ag(r, json["parametros"]);
    else if (algoritmo == "HySST") return  hysst(r, json["parametros"]);
    else if (algoritmo == "SA-ILS") return sails(r, json["parametros"]);
    else /* WDJU */ return wdju(r, json["parametros"]);
  };

  fmt::print("Algoritmo: {}\n", algoritmo);
  const auto tipo_fo = [&] {
    switch (r.horarioTipoFo)  {
      case Configuracao::TipoFo::Soma_carga:
        return "Grade";
      case Configuracao::TipoFo::Soft_constraints:
        return "Preferencias";
    }
    return "";
  }();
  fmt::print("FO: {}\n", tipo_fo);

  const auto versao_ag = [&] {
    switch (r.versaoAg)  {
      case Configuracao::Versao_AG::Serial:
        return "Serial";
      case Configuracao::Versao_AG::Paralelo:
        return "Paralelo";
    }
    return "";
  }();
  if (algoritmo == "AG") {
    fmt::print("Versao AG: {}\n", versao_ag);
  }

  const auto versao_grasp = [&] {
    switch (r.versaoGrasp) {
      case Configuracao::Versao_GRASP::Serial:
        return "Serial";
      case Configuracao::Versao_GRASP::Paralelo:
        return "Paralelo";
      case Configuracao::Versao_GRASP::ProdutorConsumidor:
        return "ProdutorConsumidor";
      case Configuracao::Versao_GRASP::Paralelo_ProdutoConsumidor:
        return "Paralelo_ProdutoConsumidor";
    }
    return "";
  }();
  if (r.horarioTipoFo == Configuracao::TipoFo::Soma_carga) {
    fmt::print("Versao GRASP: {}\n", versao_grasp);
  }

  std::vector<long long> timings(num_repetitions);
  std::vector<double> iter_timings(num_repetitions);

  fmt::print("\nN,Tempo (ms),FO,Iter,Ms/Iter\n");

  for (auto i = 0; i < num_repetitions; i++) {
    Timer t;
    const auto s = run_algorithm();

    const auto tempo = t.elapsed();
    timings[i] = tempo;
    const auto t_por_iter = tempo * 1. / r.ultimaIteracao;
    iter_timings[i] = t_por_iter;

    fmt::print("{},{},{},{},{}\n",
               i, tempo, s->getFO(), r.ultimaIteracao, t_por_iter);
  }

  fmt::print("\n");
  fmt::print("NumThreads:\n");
  fmt::print("    AG: {}\n", r.numThreadsAG());
  fmt::print("    GRASP: {}\n", r.numThreadsGRASP());
  fmt::print("    PC: {}\n\n", r.numThreadsProdutorConsumidor());
  fmt::print("Media  : {} - {}\n", average(timings), average(iter_timings));
  fmt::print("Mediana: {} - {}\n", median(timings), median(iter_timings));
}

const auto usage = R"(
Forma de usar: 
  faptp -h | --help
  faptp -i <file> -c <file> -o <file> [-m <mode>]

Onde:
  -h, --help
      Mostra essa mensagem de ajuda.

  -i, --input
      Arquivo JSON com os dados de entrada.

  -c, --config
      Arquivo JSON com os dados de configura��o do algoritmo.

  -o, --output
      Nome do arquivo de sa�da a ser gerado com os resultados.

  -m, --mode
      Modo de execu��o: 
        * horario : gera��o da matriz de hor�rios. Default.
        * grade : gera��o das grades dos alunos. A entrada deve conter
                  o hor�rio pronto.
)";

void 
check_argument(const std::string& argument, const cxxopts::ParseResult& result)
{
  if (result.count(argument) == 0) {
    fmt::print("Argumento necessario: {}", argument);
    exit(1);
  }
}

void 
check_arguments(const std::initializer_list<std::string> arguments,
                const cxxopts::ParseResult& result)
{
  for (const auto& arg : arguments) {
    check_argument(arg, result);
  }
}

int main(int argc, const char* argv[])
{
  try {
    cxxopts::Options options{
      "faPTP",
      "Gera��o de matrizes de hor�rio para institui��es de ensino superior privadas"
    };

    options.add_options()
      ("h,help", "Mostrar ajuda")
      ("i,input", "Arquivo de entrada", cxxopts::value<std::string>())
      ("c,config", "Arquivo de configura��o", cxxopts::value<std::string>())
      ("o,output", "Arquivo de sa�da", cxxopts::value<std::string>())
      ("m,mode", "Modo de execu��o", cxxopts::value<std::string>())
      ("r,repetitions", "N�mero de repeti��es", cxxopts::value<int>());

    const auto result = options.parse(argc, argv);

    if (result.count("help"))
      std::cout << usage << "\n";
    else {
      check_arguments({ "config", "input", "output" }, result);

      std::string mode{ "horario" };
      if (result.count("mode"))
        mode = result["mode"].as<std::string>();

      const auto config = result["config"].as<std::string>();
      const auto input = result["input"].as<std::string>();
      const auto output = result["output"].as<std::string>();

      if (result.count("repetitions")) {
        const auto repetitons = result["repetitions"].as<int>();
        run_many(config, input, output, mode, repetitons);
      } else {
        run(config, input, output, mode);
      }

    }

    return 0;

  } catch (const cxxopts::OptionException& e) {
    std::cout << "Erro ao ler os argumentos: " << e.what() << "\n";
    return 1;
  }
}
