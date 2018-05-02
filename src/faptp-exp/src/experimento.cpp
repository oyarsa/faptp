#include "experimento.h"

#include <chrono>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <fmt/format.h>

#include <faptp-lib/Configuracao.h>
#include <faptp-lib/HySST.h>
#include <faptp-lib/ILS.h>
#include <faptp-lib/Output.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/SA.h>
#include <faptp-lib/Timer.h>
#include <faptp-lib/Util.h>
#include <faptp-lib/WDJU.h>
#include <iostream>

#include <curl/curl.h>

/*
* PARÂMETROS DE CONFIGURAÇÃO *
* input.all.json ->
*      camadasTamanho : 33
*      perfilTamanho : 1392
* input.json (impreciso, mas funciona) ->
*      camadasTamaho : 4
*      perfilTamanho : 10
*/

namespace experimento {

struct Entrada
{
  int camadasTamanho;
  int perfilTamanho;
};

constexpr Entrada input_all_json{ 33, 1392 };
constexpr Entrada input_json{ 4, 10 };

static const auto num_tentativas_upload = 15;
static const auto segundos_espera = 30;

// número de iterações grande para o algoritmo se encerrar por tempo
constexpr auto infinito = static_cast<int>(1e9);

void
upload_result(const std::string& id,
              const std::string& resultado,
              const int num_config,
              const std::string& servidor)
{
  const auto payload = fmt::format("result={}&nome={}", resultado, id);

  for (auto i = 0; i < num_tentativas_upload; i++) {
    std::this_thread::sleep_for(std::chrono::seconds(segundos_espera));

    const auto curl = curl_easy_init();
    if (!curl) {
      throw std::logic_error{ "Impossível inicializar CURL" };
    }

    curl_easy_setopt(curl, CURLOPT_URL, servidor.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    const auto rv = curl_easy_perform(curl);

    if (rv != CURLE_OK) {
      fmt::print("{}) {}: enviado com sucesso\n\n", num_config, id);
      return;
    }

    fmt::print("{}) tentativa {} falhou: {}\n\n",
               num_config,
               i + 1,
               curl_easy_strerror(rv));
  }

  fmt::print("{}) Erro ao enviar {}. Salvo em 'falhas/'\n", num_config, id);

  const auto path = Util::join_path({ "falhas" });
  Util::create_folder(path);
  const auto filename = path + std::string{ id } + ".txt";

  std::ofstream out_file{ filename };
  out_file << resultado;
}

std::string
get_auto_file_name()
{
  auto pc_name = Util::get_computer_name();
  std::cout << "PC Name: " << pc_name << "\n";
  auto pos = pc_name.find('-');
  if (pos == std::string::npos) {
    std::cout << "Nome invalido\n";
    exit(1);
  }
  auto pc_num = std::stoi(pc_name.substr(pos + 1));
  std::cout << "PC Number: " << pc_num << "\n";

  auto filename = std::to_string(pc_num) + ".txt";
  auto path = Util::join_path({ "config" }, filename);

  std::cout << "Path: " << path << "\n";
  return path;
}

void
print_violacoes(const tsl::robin_map<std::string, int>& m)
{
  for (auto& p : m) {
    std::cout << p.first << " = " << p.second << "\n";
  }
  std::cout << "\n";
}

std::pair<long long, Solucao::FO_t>
ag(const std::string& input,
   int n_indiv,
   int taxa_mut,
   int p_cruz,
   const std::string& oper_cruz,
   int grasp_iter,
   int grasp_nviz,
   int grasp_alfa,
   int n_tour,
   int n_mut,
   long long timeout)
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
                 .numMaximoIteracoesSemEvolucaoAG(infinito)
                 .tipoCruzamento(cruzamento)
                 .mutacaoProbabilidade(taxa_mut) // %
                 .graspNumVizinhos(grasp_nviz)
                 .graspAlfa(grasp_alfa) // %
                 .camadaTamanho(input_all_json.camadasTamanho)
                 .perfilTamanho(input_all_json.perfilTamanho)
                 .numTorneioPopulacao(n_tour)
                 .tentativasMutacao(n_mut)
                 .tipoFo(Configuracao::TipoFo::Soft_constraints)
                 .timeout(timeout) };

  Timer t;
  r.gerarHorarioAG();
  auto fo = r.getSolucao()->getFO();
  auto tempo = t.elapsed();

  return { tempo, fo };
}

void
ag_cli(const std::string& input,
       const std::string& file,
       const std::string& servidor,
       long long timeout)
{
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
  if (!config) {
    fmt::print("Erro ao abrir arquivo de configuracao: {}\n", conf_file);
    return;
  }

  std::vector<std::thread> threads;

  std::string id, cruz_oper;
  int taxa_mut, n_indiv, p_cruz, n_mut, n_tour, grasp_iter;
  int grasp_nviz, grasp_alfa, n_exec;
  auto num_config = 1;

  while (config >> id >> taxa_mut >> n_indiv >> p_cruz >> cruz_oper >> n_mut >>
         n_tour >> grasp_iter >> grasp_nviz >> grasp_alfa >> n_exec) {

    std::cout << "ID: " << id << "\n\n";
    std::ostringstream out;
    out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";

    for (auto i = 0; i < n_exec; i++) {
      std::cout << i << "\n";
      const auto [tempo, fo] = ag(input,
                                 n_indiv,
                                 taxa_mut,
                                 p_cruz,
                                 cruz_oper,
                                 grasp_iter,
                                 grasp_nviz,
                                 grasp_alfa,
                                 n_tour,
                                 n_mut,
                                 timeout);

      Util::logprint(out, fmt::format("{},{},{},{}\n", id, i, tempo, fo));
      std::cout << "\n";

      threads.emplace_back(upload_result, id, out.str(), num_config, servidor);
      num_config++;
    }

    for (auto& t : threads) {
      t.join();
    }
  }
}

template<typename F>
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
                   .numMaximoIteracoesSemEvolucaoAG(infinito)
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

    Util::logprint(oss, fmt::format("i: {}", i + 1));

    Timer t;
    auto s = f(r);

    Util::logprint(oss,
                   fmt::format(" - fo: {} - t: {}\n", s->getFO(), t.elapsed()));
    Util::logprint(
      oss,
      fmt::format("\t fo_alvo: {} - tempo_alvo: {}\n", r.foAlvo, r.tempoAlvo));
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
teste_tempo(int timeout_sec)
{
  std::cout << "Teste de tempo\n\n";
  const auto timeout_ms = timeout_sec * 1000;
  const auto num_exec = 5;

  std::ostringstream oss;
  oss << std::string(25, '=') << "\n";
  oss << "Data: " << Util::date_time() << "\n";
  oss << "Tempo maximo: " << timeout_ms << "\n";
  oss << "Numero de execucoes: " << num_exec << "\n\n";

  /*Util::logprint(oss, "SA-ILS\n");
  oss << teste_tempo_iter(
  num_exec, [&](Resolucao& r) { return r.gerarHorarioSA_ILS(timeout_ms); });

  Util::logprint(oss, "HySST\n");
  oss << teste_tempo_iter(num_exec, [&](Resolucao& r) {
  return r.gerarHorarioHySST(timeout_ms, 100, 100);
  });*/

  Util::logprint(oss, "WDJU\n");
  oss << teste_tempo_iter(
    num_exec, [&](Resolucao& r) { return r.gerarHorarioWDJU(timeout_ms); });

  Util::logprint(oss, "AG\n");
  oss << teste_tempo_iter(num_exec, [&](Resolucao& r) {
    r.setTimeout(timeout_ms);
    return r.gerarHorarioAG()->clone();
  });

  std::ofstream out{ (fmt::format("resultados{}.txt", timeout_sec)),
                     std::ios::out | std::ios::app };
  out << oss.str() << "\n";
}

std::pair<long long, Solucao::FO_t>
sa_ils(const std::string& input,
       int frac_time,
       double alfa,
       double t0,
       int sa_iter,
       int sa_reaq,
       int sa_chances,
       int ils_iter,
       int ils_pmax,
       int ils_p0,
       long long timeout)
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
                 .tipoFo(Configuracao::TipoFo::Soft_constraints)
                 .perfilTamanho(input_all_json.perfilTamanho) };

  Timer t;
  SA sa{ r, alfa, t0, sa_iter, sa_reaq, timeout / frac_time, escolhido };
  ILS ils{ r, infinito, ils_pmax, ils_p0, ils_iter, timeout / frac_time };
  auto s = r.gerarHorarioSA_ILS(sa, ils, timeout);
  auto fo = s->getFO();
  auto tempo = t.elapsed();

  const auto violacoes = s->reportarViolacoes();
  for (const auto& p : violacoes) {
    fmt::print("{}: {}\n", p.first, p.second);
  }

  return { tempo, fo };
}

void
sa_ils_cli(const std::string& input,
           const std::string& file,
           const std::string& servidor,
           long long timeout)
{
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
      const auto[tempo, fo] = sa_ils(input,
                                     frac_time,
                                     alfa,
                                     t0,
                                     sa_iter,
                                     sa_reaq,
                                     sa_chances,
                                     ils_iter,
                                     ils_pmax,
                                     ils_p0,
                                     timeout);

      Util::logprint(out_str, fmt::format("{},{},{},{}\r\n", id, i, tempo, fo));
    }
    std::cout << "\n";

    threads.emplace_back(
      upload_result, id, out_str.str(), num_config, servidor);
    num_config++;
  }

  for (auto& t : threads) {
    t.join();
  }
}

std::pair<long long, Solucao::FO_t>
hysst(const std::string& input,
      int max_level,
      int t_start,
      int t_step,
      int it_hc,
      int it_mut,
      long long timeout)
{
  Resolucao r{ Configuracao()
                 .arquivoEntrada(input)
                 .camadaTamanho(input_all_json.camadasTamanho)
                 .perfilTamanho(input_all_json.perfilTamanho)
                 .tipoFo(Configuracao::TipoFo::Soft_constraints)
                 .tentativasMutacao(it_mut) };

  Timer t;
  HySST hysst{ r, timeout, 100, 100, max_level, t_start, t_step, it_hc };
  auto s = r.gerarHorarioHySST(hysst, it_mut);
  auto fo = s->getFO();
  auto tempo = t.elapsed();

  return { tempo, fo };
}

void
hysst_cli(const std::string& input,
          const std::string& file,
          const std::string& servidor,
          long long timeout)
{
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
    std::cout << "ID: " << id << "\n\n";
    std::ostringstream out;
    out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";

    for (auto i = 0; i < n_exec; i++) {
      const auto[tempo, fo] =
        hysst(input, max_level, t_start, t_step, it_hc, it_mut, timeout);

      Util::logprint(out, fmt::format("{},{},{},{}\n", id, i, tempo, fo));
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
wdju(const std::string& input,
     int stag_limit,
     double jump_factor,
     long long timeout)
{
  Resolucao r{ Configuracao()
                 .arquivoEntrada(input)
                 .camadaTamanho(input_all_json.camadasTamanho)
                 .tipoFo(Configuracao::TipoFo::Soft_constraints)
                 .perfilTamanho(input_all_json.perfilTamanho) };

  Timer t;
  WDJU wdju{ r, timeout, stag_limit, jump_factor };
  auto s = r.gerarHorarioWDJU(wdju);
  auto fo = s->getFO();
  auto tempo = t.elapsed();

  return { tempo, fo };
}

void
wdju_cli(const std::string& input,
         const std::string& file,
         const std::string& servidor,
         long long timeout)
{
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
    std::cout << "ID: " << id << "\n\n";
    std::ostringstream out;
    out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";

    for (auto i = 0; i < n_exec; i++) {
      const auto[tempo, fo] = wdju(input, stag_limit, jump_factor, timeout);

      Util::logprint(out, fmt::format("{},{},{},{}\n", id, i, tempo, fo));
    }
    std::cout << "\n";

    threads.emplace_back(upload_result, id, out.str(), num_config, servidor);
    num_config++;
  }

  for (auto& t : threads) {
    t.join();
  }
}

template<typename F>
void
comparacao_iter(int num_exec, const std::string& nome, F heuristica)
{
  constexpr auto k_headers = "Algoritmo,FO";

  std::ostringstream saida{};
  saida << k_headers << "\n";

  for (auto i = 0; i < num_exec; i++) {
    Resolucao r{ Configuracao()
                   .arquivoEntrada(
                     Util::join_path({ "entradas" }, "input.all.json"))
                   .populacaoInicial(20)
                   .porcentagemCruzamentos(80)
                   .numMaximoIteracoesSemEvolucaoGRASP(15)
                   .numMaximoIteracoesSemEvolucaoAG(infinito)
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
                   .tipoFo(Configuracao::TipoFo::Soma_carga) };

    auto s = heuristica(r);
    saida << nome << "," << s->getFO() << "\n";
    std::cout << "\t" << s->getFO() << "\n";
  }
  std::cout << "\n";

  std::ofstream arquivo{ std::string{ nome } + ".csv" };
  arquivo << saida.str();
}

void
comparacao(int timeout_sec = 60, int num_exec = 30)
{
  const auto timeout_ms = timeout_sec * 1000;

  std::cout << "SA-ILS\n";
  comparacao_iter(num_exec, "SA-ILS", [&](Resolucao& r) {
    SA sa{ r,
           0.97,
           2,
           100,
           500,
           timeout_ms / 100,
           { { Resolucao::Vizinhanca::ES, 25 },
             { Resolucao::Vizinhanca::EM, 43 },
             { Resolucao::Vizinhanca::RS, 20 },
             { Resolucao::Vizinhanca::RM, 10 },
             { Resolucao::Vizinhanca::KM, 2 } } };
    ILS ils{ r, infinito, 30, 3, 10, timeout_ms / 100 };
    return r.gerarHorarioSA_ILS(sa, ils, timeout_ms);
  });

  std::cout << "HySST\n";
  comparacao_iter(num_exec, "HySST", [&](Resolucao& r) {
    HySST hysst{ r, timeout_ms, 100, 100, 15, 1, 10, 5 };
    return r.gerarHorarioHySST(hysst, 5);
  });

  std::cout << "WDJU\n";
  comparacao_iter(num_exec, "WDJU", [&](Resolucao& r) {
    WDJU wdju{ r, timeout_ms, 30, 0.002 };
    return r.gerarHorarioWDJU(wdju);
  });

  /*
  std::cout << "AG\n";
  comparacao_iter(num_exec, "AG", [&](Resolucao& r) {
  r.setTimeout(timeout_ms);
  return r.gerarHorarioAG()->clone();
  });
  */
}

} // namespace experimento
