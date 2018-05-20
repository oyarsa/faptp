#include <cxxopts.hpp>
#include <fmt/format.h>
#include <curl/curl.h>

#include "experimento.h"

constexpr int infinito = static_cast<int>(1e6);

const auto usage = R"(
Usage:
    faptp -h | --help
    faptp <-a|--algo ALGO> <-i|--input INPUT> <-c|--config CONF> <-s|--server SERVER>
    faptp

Onde:
    -h, --help
        Mostra esta mensagem de ajuda e encerra o programa.

    ALGO
        Algoritmo a ser executado. Deve ser um entre: ag, sa_ils, hysst, wdju.

    INPUT
        Arquivo de entrada.

    CONF
        Arquivo de configuracao. Se for 'auto', ira pegar o numero do arquivo
        de acordo com o numero da maquina, buscando numa pasta 'config'. Por
        exemplo, uma maquina de nome 'XXX-01' ira gerar o caminho 'config\1.txt'.

    SERVER
        Endereco IP do servidor para onde os resultados serao enviados.

    <Nenhuma opcao>
        Execucao dos testes.
)";

int
main(int argc, char** argv)
{
   curl_global_init(CURL_GLOBAL_ALL);

   if (argc == 1) {
       fmt::print("Sem argumentos. Executando: ");
       experimento::teste_tempo(1 * 60);
       // experimento::teste_tempo(3 * 60);
       return 0;
   }

   try {
     cxxopts::Options options{"faPTP",
       "Geração de matrizes de horário para instituições de ensino "
       "superior privadas"};

     options.add_options()
       ("h,help", "Mostrar ajuda")
       ("a,algo", "Algoritmo", cxxopts::value<std::string>())
       ("i,input", "Arquvo de entrada", cxxopts::value<std::string>())
       ("c,config", "Arquivo de configuracao",
        cxxopts::value<std::string>()->default_value("auto"))
       ("s,server", "Servidor para submissao dos resultados",
        cxxopts::value<std::string>())
       ("t,timeout", "Tempo maximo para cada execucao de um algoritmo",
        cxxopts::value<int>())  ;

     const auto result = options.parse(argc, argv);

     if (result.count("help")) {
       std::cout << usage << "\n";
       return 0;
     }

     const auto algo = result["algo"].as<std::string>();
     const auto entrada = result["input"].as<std::string>();
     const auto configuracao = result["config"].as<std::string>();
     const auto servidor = result["server"].as<std::string>();

     const auto timeout_s = result.count("timeout") ? result["timeout"].as<int>() : infinito;
     const auto timeout = timeout_s * 1000;

     if (algo == "ag") {
       experimento::ag_cli(entrada, configuracao, servidor, timeout);
     } else if (algo == "sa_ils") {
       experimento::sa_ils_cli(entrada, configuracao, servidor, timeout);
     } else if (algo == "hysst") {
       experimento::hysst_cli(entrada, configuracao, servidor, timeout);
     } else if (algo == "wdju") {
       experimento::wdju_cli(entrada, configuracao, servidor, timeout);
     } else if (algo == "grasp") {
       experimento::grasp_cli(entrada, configuracao, servidor, timeout);
     } else {
       std::cout << "Algoritmo invalido\n";
       return 1;
     }
     return 0;

   } catch (const cxxopts::OptionException& e) {
     fmt::print("Erro ao ler argumentos: {}", e.what());
     return 1;
   }

}
