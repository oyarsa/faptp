#include <cxxopts.hpp>
#include <fmt/format.h>

#include <faptp-exp/experimento.h>

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
main(int argc, char* argv[])
{
   const auto timeout = 60 * 60 * 1000;

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
        cxxopts::value<std::string>());
     
     options.parse(argc, argv);

     if (options.count("help")) {
       std::cout << usage << "\n";
       return 0;
     }

     auto algo = options["algo"].as<std::string>();
     auto entrada = options["input"].as<std::string>();
     auto configuracao = options["config"].as<std::string>();
     auto servidor = options["server"].as<std::string>();

     if (algo == "ag") {
       experimento::ag_cli(entrada, configuracao, servidor, timeout);
     } else if (algo == "sa_ils") {
       experimento::sa_ils_cli(entrada, configuracao, servidor, timeout);
     } else if (algo == "hysst") {
       experimento::hysst_cli(entrada, configuracao, servidor, timeout);
     } else if (algo == "wdju") {
       experimento::wdju_cli(entrada, configuracao, servidor, timeout);
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
