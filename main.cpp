#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <chrono>

#include "src/parametros.h"
#include "src/Resolucao.h"
#include "src/Output.h"
#include "src/Configuracao.h"
#include "src/Util.h"

void semArgumentos()
{
    experimento = false;

    Resolucao r {Configuracao()
            .arquivoEntrada(Util::join_path({"res"}, "input.all.json"))
            .populacaoInicial(20)
            .porcentagemCruzamentos(30) // %
            .numMaximoIteracoesSemEvolucaoGRASP(15)
            .numMaximoIteracoesSemEvolucaoAG(20)
            .tipoCruzamento(Configuracao::TipoCruzamento::ciclo)
            .tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
            .mutacaoProbabilidade(15) // %
            .graspNumVizinhos(2)
            .graspAlfa(20) // %
            .camadaTamanho(33)
            .perfilTamanho(1392)
            .numTorneioPopulacao(4)
            .tentativasMutacao(4)
            .graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
            .tipoConstrucao(Configuracao::TipoGrade::grasp)
            };

    std::cout << "Montando horarios [AG + Modelo]...\n";

    auto inicio = Util::now();
    r.gerarHorarioAG();
    auto fim = Util::now();

    std::cout << "Tempo do horario: " << Util::chronoDiff(fim, inicio) << "ms\n\n";

    auto fo = r.getSolucao()->getFO();
    std::cout << "\nResultado:" << fo << "\n";
    //r.showResult();

    auto savePath = Util::join_path({"teste", "fo" + std::to_string(fo)});
    Output::write(r.getSolucao(), savePath);

    std::ostringstream oss;
    oss << "solucaoAlvo: " << r.foAlvo << "\n";
    oss << "hashAlvo: " << r.hashAlvo << "\n";
    oss << "iteracoes: " << r.iteracaoAlvo << "\n";
    oss << "tempoAlvo: " << r.tempoAlvo << "\n";
    oss << "ultima iteracao: " << r.ultimaIteracao << "\n\n";
    std::cout << oss.str();

    std::ofstream out {savePath + "log.txt"};
    //r.logExperimentos();
    out << oss.str();
    out << r.getLog();
}

void teste()
{
    Resolucao r {Configuracao()
            .arquivoEntrada(Util::join_path({"res"}, "input.all.json"))
            .populacaoInicial(1)
            .numIteracoes(0)
            .porcentagemSolucoesAleatorias(0) // %
            .porcentagemCruzamentos(0) // %
            .numMaximoIteracoesSemEvolucaoGRASP(25)
            .tipoCruzamento(Configuracao::TipoCruzamento::ciclo)
            .tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
            .mutacaoProbabilidade(30) // %
            .graspTempoConstrucao(600) // ms
            .graspNumVizinhos(2)
            .graspAlfa(30) // %
            .camadaTamanho(40)
            .perfilTamanho(1400)
            .numTorneioPares(0)
            .numTorneioPopulacao(1)
            .tentativasMutacao(2)
            .graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
            .tipoConstrucao(Configuracao::TipoGrade::modelo)
            //.tipoConstrucao(Configuracao::TipoGrade::grasp)
            };

    r.teste();
}

std::pair<long long, double>
novo_experimento(const std::string& input, const std::string& id,
                 int n_indiv, int taxa_mut, int p_cruz,
                 const std::string& oper_cruz,
                 int grasp_iter, int ag_iter,
                 int grasp_nviz, int grasp_alfa, int n_tour, int n_mut,
                 int exec_i)
{
    experimento = false;

    auto timenow = Output::timestamp();
    auto execstr = "Exec" + std::to_string(exec_i);

    Configuracao::TipoCruzamento cruz {};
    if (oper_cruz == "PMX")
        cruz = Configuracao::TipoCruzamento::pmx;
    else if (oper_cruz == "CX")
        cruz = Configuracao::TipoCruzamento::ciclo;
    else if (oper_cruz == "OX")
        cruz = Configuracao::TipoCruzamento::ordem;
    else
        cruz = Configuracao::TipoCruzamento::pmx;

    Resolucao r {Configuracao()
            .arquivoEntrada(input)
            .populacaoInicial(n_indiv)
            .porcentagemCruzamentos(p_cruz) // %
            .numMaximoIteracoesSemEvolucaoGRASP(grasp_iter)
            .numMaximoIteracoesSemEvolucaoAG(ag_iter)
            .tipoCruzamento(cruz)
            .tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
            .mutacaoProbabilidade(taxa_mut) // %
            .graspNumVizinhos(grasp_nviz)
            .graspAlfa(grasp_alfa) // %
            .camadaTamanho(33)
            .perfilTamanho(1400)
            .numTorneioPares(0)
            .numTorneioPopulacao(n_tour)
            .tentativasMutacao(n_mut)
            .graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
            .tipoConstrucao(Configuracao::TipoGrade::grasp)
            };

    auto inicio = Util::now();
    r.gerarHorarioAG();
    auto fim = Util::now();
    auto tempo = Util::chronoDiff(fim, inicio);
    auto fo = r.getSolucao()->getFO();

    return {tempo, fo};
}

std::pair<long long, double>
novo_experimento_fase2(const std::string& input, const std::string& id, int exec_i,
                       int ag_iter, Configuracao::TipoCruzamento tipo_cruz)
{
    auto timenow = Output::timestamp();
    auto execstr = "Exec" + std::to_string(exec_i);
    //auto path = Util::join_path({"exper", "log", id + execstr});
    //Util::create_folder(path);
    //auto fileout = path + "result" + id + execstr + ".txt";
    //std::cout << id << "\n";
    //std::cout << "File: " << fileout << "\n";

    Resolucao r {Configuracao()
            .arquivoEntrada(input)
            .populacaoInicial(40)
            .porcentagemCruzamentos(30) // %
            .numMaximoIteracoesSemEvolucaoGRASP(25)
            .numMaximoIteracoesSemEvolucaoAG(ag_iter)
            .tipoCruzamento(tipo_cruz)
            .tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
            .mutacaoProbabilidade(30) // %
            .graspNumVizinhos(2)
            .graspAlfa(60) // %
            .camadaTamanho(40)
            .perfilTamanho(1400)
            .numTorneioPares(0)
            .numTorneioPopulacao(4)
            .tentativasMutacao(2)
            .graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
            .tipoConstrucao(Configuracao::TipoGrade::grasp)
            };

    auto inicio = Util::now();
    r.gerarHorarioAG();
    auto fim = Util::now();
    auto tempo = Util::chronoDiff(fim, inicio);

    //auto saida = std::ofstream(fileout);
    //saida << id << "\n";
    //saida << execstr << "\n";
    //saida << "Tempo do horario: " << tempo << "ms\n";

    auto fo = r.getSolucao()->getFO();
    //saida << "\nResultado:" << fo << std::endl;

    //saida << "hash: " << r.hashAlvo << "\n";
    //saida << "solucaoAlvo:" << r.foAlvo << "\n";
    //saida << "iteracoes:" << r.iteracaoAlvo << "\n";
    //saida << "tempoAlvo:" << r.tempoAlvo << "\n";
    //saida << "ultima iteracao: " << r.ultimaIteracao << "\n\n";
    //saida << r.getLog() << "\n";

    //Output::write(r.getSolucao(), path);

    return {tempo, fo};
}

void novo_experimento_cli(const std::string& input, const std::string& file)
{
    std::ifstream config {file};

    // comentário do topo
    // formato entrada:
    // ID AGIter TaxaMut NIndiv %Cruz CruzOper NMut NTour GRASPIter GRASPNVzi GRASPAlfa NExec
    // formato saida:
    // ID,NExec,Tempo,Fo

    std::string id, cruz_oper;
    int ag_iter, taxa_mut, n_indiv, p_cruz, n_mut, n_tour, grasp_iter;
    int grasp_nviz, grasp_alfa, n_exec;

    while (config >> id >> ag_iter >> taxa_mut >> n_indiv >> p_cruz
        >> cruz_oper >> n_mut >> n_tour
        >> grasp_iter >> grasp_nviz >> grasp_alfa >> n_exec) {

        auto path = Util::join_path({"experimento"});
        Util::create_folder(path);

        auto filename = path + id + ".txt";
        std::ofstream out {filename};

        std::cout << "\n\nID: " << id << "\n";

        out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";
        for (auto i = 0; i < n_exec; i++) {
            long long tempo;
            double fo;
            std::tie(tempo, fo) = novo_experimento(
                input, id, n_indiv, taxa_mut, p_cruz, cruz_oper, grasp_iter,
                ag_iter, grasp_nviz, grasp_alfa, n_tour, n_mut, i);

            std::cout << id << "," << i << "," << tempo << "," << fo << "\n";
            out << id << "," << i << "," << tempo << "," << fo << "\n";
        }
    }
}

void novo_experimento_cli_fase2(const std::string& input, const std::string& file)
{
    experimento = true;
    std::ifstream config {file};

    // comentário do topo
    // formato entrada:
    // ID AGIter NIndiv %Cruz NMut NTour GRASPIter GRASPNVzi GRASPAlfa NExec
    // formato saida:
    // ID,NExec,Tempo,Fo

    std::string id {};
    int ag_iter {};
    int xover_id {};
    int n_exec {};
    Configuracao::TipoCruzamento xover {};

    while (config >> id >> ag_iter >> xover_id >> n_exec) {
        switch (xover_id) {
            case 1:
                xover = Configuracao::TipoCruzamento::pmx;
                break;
            case 2:
                xover = Configuracao::TipoCruzamento::ordem;
                break;
            case 3:
                xover = Configuracao::TipoCruzamento::ciclo;
                break;
            default:
                xover = Configuracao::TipoCruzamento::pmx;
                break;
        }

        auto path = Util::join_path({"exper", "result"});
        Util::create_folder(path);

        auto filename = path + "result" + id + ".txt";
        std::ofstream out {filename};

        std::cout << "\n\nID: " << id << "\n";

        out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";
        for (auto i = 0; i < n_exec; i++) {
            long long tempo;
            double fo;
            std::tie(tempo, fo) = novo_experimento_fase2(
                input, id, i, ag_iter, xover);

            std::cout << id << "," << i << "," << tempo << "," << fo << "\n";
            out << id << "," << i << "," << tempo << "," << fo << "\n";

            // TODO:
            // Refatorar os métodos de crossover (DRY)
        }
    }
}

int main(int argc, char* argv[])
{
    verbose = false;
    if (argc == 3) {
        // Primeiro argumento é a entrada, o segundo é o arquivo de configuração
        //novo_experimento_cli(argv[1], argv[2]);
        semArgumentos();
    } else if (argc == 2) {
        std::string flag = argv[1];
        if (flag == "-h" || flag == "--help") {
            std::cout << "Primeiro argumento é a entrada, o segundo é o arquivo de configuração\n";
            std::cout << "Formato da config:\n";
            std::cout << "ID AGIter NIndiv %Cruz NMut NTour GRASPIter GRASPNVzi GRASPAlfa NExec\n";
        }
    } else {
        semArgumentos();
    }
}

