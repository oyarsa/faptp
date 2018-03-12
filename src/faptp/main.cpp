#include <faptp-lib/Configuracao.h>
#include <faptp-lib/HySST.h>
#include <faptp-lib/ILS.h>
#include <faptp-lib/Output.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/SA.h>
#include <faptp-lib/Semana.h>
#include <faptp-lib/WDJU.h>

#include <faptp-lib/DadosModelo.h>
#include <faptp-lib/Modelo.h>

#include <fstream>
#include <iostream>
#include <memory>

#include <json/json.h>
#include <cxxopts.hpp>
#include <fmt/format.h>

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
    auto x = json["CruzOper"];
    if (x == "CX") return Configuracao::TipoCruzamento::ciclo;
    if (x == "OX") return Configuracao::TipoCruzamento::ordem;
    else /* PMX */ return Configuracao::TipoCruzamento::pmx; 
  }();
  r.horarioMutacaoProbabilidade = json["TaxaMut"].asInt() / 100.0;

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
  const auto numThreads = [&json]{
    if (json.isMember("NumThreads")) {
      return json["NumThreads"].asInt();
    }
    return 1;
  }();

  Resolucao r{ Configuracao()
    .arquivoEntrada(input)
    .blocoTamanho(numeroHorarios)
    .camadaTamanho(numeroPeriodos)
    .perfilTamanho(numeroAlunos)
    .timeout(timeout * 1000)
    .tipoFo(fo)
    .numThreads(numThreads) };

  /*// TODO: Remover
  auto dados = DadosModelo{ r };
  auto rv = modelo(dados);*/

  r.gradeAlfa = json["parametros"]["GAlfa"].asInt();
  r.maxIterSemEvoGrasp = json["parametros"]["GIter"].asInt();
  r.gradeGraspVizinhos = json["parametros"]["GNViz"].asInt();

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
    Output::writeError("Solução infactível", out);
    return;
  }

  r.gerarGrade(solucao.get());
  Output::writeJson(*solucao, out);
}

long long average(const std::vector<long long>& timings)
{
  return std::accumulate(begin(timings), end(timings), 0ll) / timings.size();
}

long long median(std::vector<long long> v)
{
  std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
  return v[v.size() / 2];
}

void run_many(const std::string& conf, const std::string& input,
              const std::string& out, const std::string& mode, 
              const int num_repetitions)
{
  (void) out;
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
  const auto num_threads = [&json] {
    if (json.isMember("NumThreads")) {
      return json["NumThreads"].asInt();
    }
    return 1;
  }();

  Resolucao r{ Configuracao()
    .arquivoEntrada(input)
    .blocoTamanho(numero_horarios)
    .camadaTamanho(numero_periodos)
    .perfilTamanho(numero_alunos)
    .timeout(timeout * 1000)
    .tipoFo(fo)
    .numThreads(num_threads) };

  r.gradeAlfa = json["parametros"]["GAlfa"].asInt();
  r.maxIterSemEvoGrasp = json["parametros"]["GIter"].asInt();
  r.gradeGraspVizinhos = json["parametros"]["GNViz"].asInt();

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

  std::vector<long long> timings(num_repetitions);

  for (auto i = 0; i < num_repetitions; i++) {
    Timer t;
    const auto s = run_algorithm();
    timings[i] = t.elapsed();
    fmt::print("{}: {}ms - FO: {}\n", i, t.elapsed(), s->getFO());
  }

  fmt::print("\n");
  fmt::print("Media  : {}\n", average(timings));
  fmt::print("Mediana: {}\n", median(timings));
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
      Arquivo JSON com os dados de configuração do algoritmo.

  -o, --output
      Nome do arquivo de saída a ser gerado com os resultados.

  -m, --mode
      Modo de execução: 
        * horario : geração da matriz de horários. Default.
        * grade : geração das grades dos alunos. A entrada deve conter
                  o horário pronto.
)";

int main(int argc, const char* argv[])
{
  try {
    cxxopts::Options options{
      "faPTP",
      "Geração de matrizes de horário para instituições de ensino superior privadas"
    };

    options.add_options()
      ("h,help", "Mostrar ajuda")
      ("i,input", "Arquivo de entrada", cxxopts::value<std::string>())
      ("c,config", "Arquivo de configuração", cxxopts::value<std::string>())
      ("o,output", "Arquivo de saída", cxxopts::value<std::string>())
      ("m,mode", "Modo de execução", cxxopts::value<std::string>())
      ("r,repetitions", "Número de repetições", cxxopts::value<int>());

    const auto result = options.parse(argc, argv);

    if (result.count("help"))
      std::cout << usage << "\n";
    else {
      std::string mode{ "horario" };
      if (result.count("mode"))
        mode = result["mode"].as<std::string>();

      if (result.count("repetitions")) {
        run_many(result["config"].as<std::string>(), 
                result["input"].as<std::string>(),
                result["output"].as<std::string>(),
                mode, result["repetitions"].as<int>());
      } else {
        run(result["config"].as<std::string>(), 
            result["input"].as<std::string>(),
            result["output"].as<std::string>(),
            mode);
      }

    }

    return 0;

  } catch (const cxxopts::OptionException& e) {
    std::cout << "Erro ao ler os argumentos: " << e.what() << "\n";
    return 1;
  }
}
