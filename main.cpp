#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <chrono>

#include <modelo-grade/modelo_solver.h>

#include "src/parametros.h"
#include "src/Resolucao.h"
#include "src/Output.h"
#include "src/Configuracao.h"
#include "src/Util.h"


void comArgumentos(char** argv)
{
	experimento = true;

	int numVizinhos;
	double tempoConstr, alfaGrasp;

	std::ifstream arquivoConf(argv[1]);
	std::string arquivoEntrada = argv[2];

	arquivoConf >> numVizinhos >> tempoConstr >> alfaGrasp;

	Resolucao resolucaoGrasp(3, 2, 5, Configuracao::TipoGrade::grasp, arquivoEntrada);

	resolucaoGrasp.gradeGraspVizinhanca = Configuracao::TipoVizinhos::aleatorios;

	resolucaoGrasp.gradeGraspVizinhos = numVizinhos;
	resolucaoGrasp.gradeGraspTempoConstrucao = tempoConstr;

	resolucaoGrasp.gradeAlfa = alfaGrasp;

	auto inicioTempo = std::chrono::steady_clock::now();
	auto fo = resolucaoGrasp.start();
	auto fimTempo = std::chrono::steady_clock::now();

	std::cout << fo << " " << std::chrono::duration_cast<std::chrono::milliseconds>
			(fimTempo - inicioTempo).count() << "\n";
}


void calibracao()
{
	experimento = false;

	double fo;
	int p = 0;
	int params[][5] = {
		{50, 30, 120, 4, 30},
		{100, 30, 120, 4, 30},

		{50, 60, 120, 4, 30},
		{100, 60, 120, 4, 30},

		{50, 30, 120, 6, 30},
		{100, 30, 120, 6, 30},

		{50, 60, 120, 6, 30},
		{100, 60, 120, 6, 30},

		{50, 30, 120, 4, 60},
		{100, 30, 120, 4, 60},

		{50, 60, 120, 4, 60},
		{100, 60, 120, 4, 60},

		{50, 30, 120, 6, 60},
		{100, 30, 120, 6, 60},

		{50, 60, 120, 6, 60},
		{100, 60, 120, 6, 60}
	};

	for (int j = 0; j < 16; j++) {
		p++;

		if (j != 8) {
			continue;
		}

		for (int i = 0; i < 10; i++) {

			//Resolucao resolucaoGrasp(4, (50 - 15), 1413);
			Resolucao resolucaoGrasp(2, 2, 5, Configuracao::TipoGrade::grasp);

			resolucaoGrasp.horarioPopulacaoInicial = params[j][0];

			resolucaoGrasp.horarioIteracao = 100;
			resolucaoGrasp.horarioTorneioPares = 0;
			resolucaoGrasp.horarioTorneioPopulacao = 1;

			double mutacao = (double) params[j][1] / 100;
			resolucaoGrasp.horarioMutacaoProbabilidade = mutacao;
			resolucaoGrasp.horarioMutacaoTentativas = 2;

			resolucaoGrasp.gradeGraspVizinhanca = Configuracao::TipoVizinhos::aleatorios;

			double tempo = (double) params[j][2] / 100;
			resolucaoGrasp.gradeGraspTempoConstrucao = tempo;
			resolucaoGrasp.gradeGraspVizinhos = params[j][3];
			double alfa = (double) params[j][4] / 100;
			resolucaoGrasp.gradeAlfa = alfa;

			std::cout << "Inicio: Execucao " << (i + 1) << " da configuracao " << p << std::endl;

			auto inicioHorario = clock();
			resolucaoGrasp.start(false);
			auto fimHorario = clock();

			fo = resolucaoGrasp.getSolucao()->getFO();
			std::cout << "FO da solucao: " << fo << std::endl;

			double diff = ((double) (fimHorario - inicioHorario) / 1000 / 1000);
			std::cout << "Tempo do horario: " << (diff) << "s" << std::endl << std::endl;

			auto savePath = "experimento/p" + std::to_string(j) + std::to_string(i)
				+ "fo" + std::to_string(fo);
			Output::write(resolucaoGrasp.getSolucao(), savePath);
		}
	}
}

void semArgumentos()
{
	experimento = false;

	Resolucao r{Configuracao()
		.arquivoEntrada(Util::join_path({"res"}, "input.all.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input_gigante2.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input_maroto3.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input.json"))
		.populacaoInicial(10)
		.porcentagemCruzamentos(20) // %
		.numMaximoIteracoesSemEvolucaoGRASP(25)
		.numMaximoIteracoesSemEvolucaoAG(20)
		.tipoCruzamento(Configuracao::TipoCruzamento::pmx)
		//.tipoCruzamento(Configuracao::TipoCruzamento::ciclo)
		//.tipoCruzamento(Configuracao::TipoCruzamento::ordem)
		.tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
		.mutacaoProbabilidade(20) // %
		.graspNumVizinhos(3)
		.graspAlfa(40) // %
		.camadaTamanho(40)
		.perfilTamanho(1400)
		.numTorneioPares(0)
		.numTorneioPopulacao(4)
		.tentativasMutacao(5)
		.graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
		//.tipoConstrucao(Configuracao::TipoGrade::modelo)
		.tipoConstrucao(Configuracao::TipoGrade::grasp)
	};

	std::cout << "Montando horarios [AG + Modelo]..." << std::endl;

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

	std::ofstream out{savePath + "log.txt"};
	//r.logExperimentos();
	out << oss.str();
	out << r.getLog();
}


void exper(const std::string& filein, 
		   Configuracao::TipoMutacao mut, Configuracao::TipoCruzamento cruz,
		   Configuracao::TipoGrade grade, int numindividuos, 
		   int percent_cruz, int numiterag, int numitergrasp)
{
	experimento = false;
	auto timenow = Output::timestamp();
	auto path = Util::join_path({"exper", timenow});
	Util::create_folder(path);
	auto fileout = path + "result.txt";
	std::cout << "File: " << fileout << "\n";

	Resolucao r{Configuracao()
		.arquivoEntrada(filein)
		.populacaoInicial(numindividuos)
		.porcentagemCruzamentos(0) // %
		.tipoCruzamento(cruz)
		.tipoMutacao(mut)
		.mutacaoProbabilidade(20) // %
		.graspNumVizinhos(2)
		.graspAlfa(40) // %
		.numMaximoIteracoesSemEvolucaoAG(numiterag)
		.numMaximoIteracoesSemEvolucaoGRASP(numitergrasp)
		.numTorneioPares(0)
		.numTorneioPopulacao(3)
		.tentativasMutacao(2)
		.graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
		.tipoConstrucao(grade)
		.camadaTamanho(40)
		.perfilTamanho(1400)
	};

	auto inicio = std::chrono::steady_clock::now();
	r.start(false);
	auto fim = std::chrono::steady_clock::now();

	auto saida = std::ofstream(fileout);
	saida << "Tempo do horario: " << std::chrono::duration_cast<std::chrono::milliseconds>
		(fim - inicio).count() << "ms\n";

	auto fo = r.getSolucao()->getFO();
	saida << "\nResultado:" << fo << std::endl;
	//resolucaoGrasp.showResult();

	saida << "hash: " << r.hashAlvo << "\n";
	saida << "solucaoAlvo:" << r.foAlvo << "\n";
	saida << "iteracoes:" << r.iteracaoAlvo << "\n";
	saida << "tempoAlvo:" << r.tempoAlvo << "\n";
	saida << "ultima iteracao: " << r.ultimaIteracao << "\n\n";
	saida << r.getLog() << "\n";

	Output::write(r.getSolucao(), path);
}


std::pair<long long, double>
novo_experimento(const std::string& input, const std::string& id, 
				 int n_indiv, int p_cruz, int grasp_iter, int ag_iter, 
				 int grasp_nviz, int grasp_alfa, int n_tour, int n_mut,
				 int exec_i)
{
	experimento = false;

	auto timenow = Output::timestamp();
	auto execstr = "Exec" + std::to_string(exec_i);
	//auto path = Util::join_path({"exper", "log", id + execstr});
	//Util::create_folder(path);
	//auto fileout = path + "result" + id + execstr + ".txt";
	//std::cout << id << "\n";
	//std::cout << "File: " << fileout << "\n";

	Resolucao r{Configuracao()
		.arquivoEntrada(input)
		.populacaoInicial(n_indiv)
		.porcentagemCruzamentos(p_cruz) // %
		.numMaximoIteracoesSemEvolucaoGRASP(grasp_iter)
		.numMaximoIteracoesSemEvolucaoAG(ag_iter)
		.tipoCruzamento(Configuracao::TipoCruzamento::pmx)
		.tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
		.mutacaoProbabilidade(20) // %
		.graspNumVizinhos(grasp_nviz)
		.graspAlfa(grasp_alfa) // %
		.camadaTamanho(40)
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

	Resolucao r{Configuracao()
		.arquivoEntrada(input)
		.populacaoInicial(40)
		.porcentagemCruzamentos(30) // %
		.numMaximoIteracoesSemEvolucaoGRASP(25)
		.numMaximoIteracoesSemEvolucaoAG(ag_iter)
		.tipoCruzamento(tipo_cruz)
		.tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
		.mutacaoProbabilidade(20) // %
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

	return{tempo, fo};
}


void teste()
{
	Resolucao resolucaoGrasp{Configuracao()
		//.arquivoEntrada(Util::join_path({"res"}, "input_gigante.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input_gigante2.json"))
		.arquivoEntrada(Util::join_path({"res"}, "input.all.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input_maroto3.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input.json"))
		.populacaoInicial(1)
		.numIteracoes(0)
		.porcentagemSolucoesAleatorias(0) // %
		.porcentagemCruzamentos(0) // %
		.numMaximoIteracoesSemEvolucaoGRASP(5)
		.tipoCruzamento(Configuracao::TipoCruzamento::substitui_bloco)
		//.tipoCruzamento(Configuracao::TipoCruzamento::simples)
		//.tipoCruzamento(Configuracao::TipoCruzamento::construtivo_reparo)
		//.tipoMutacao(Configuracao::TipoMutacao::substitui_professor)
		.tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
		.mutacaoProbabilidade(20) // %
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

	resolucaoGrasp.teste();
}

void menu(std::string filein)
{
	int num;
	std::ofstream dados{"parametros.txt"};
	std::cout << "Rodar quantas vezes? ";
	std::cin >> num;

	auto mut = Configuracao::TipoMutacao::substiui_disciplina;
	Configuracao::TipoGrade grade;
	Configuracao::TipoCruzamento cruz;
	
	std::cout << "\n";

	int optcruz;
	std::cout << "Operador de cruzamento:\n";
	std::cout << "1 - PMX\n";
	std::cout << "2 - CX\n";
	std::cout << "3 - OX\n";
	std::cout << "Opcao> ";
	std::cin >> optcruz;

	std::cout << "\n";

	int optgrade;
	std::cout << "Geracao da grade:\n";
	std::cout << "1 - Modelo\n";
	std::cout << "2 - GRASP\n";
	std::cout << "Opcao> ";
	std::cin >> optgrade;

	std::cout << "\n";

	int numindividuos, numiterag, numitergrasp, percent_cruz;
	std::cout << "Quantos individuos? ";
	std::cin >> numindividuos;
	std::cout << "Quantas iteracoes sem melhoria (AG)? ";
	std::cin >> numiterag;
	std::cout << "Quantas iteracoes sem melhoria (GRASP)? ";
	std::cin >> numitergrasp;
	std::cout << "Porcentagem de cruzamentos por geracao? ";
	std::cin >> percent_cruz;

	std::cout << "\n\n";

	switch (optcruz) {
	case 1: cruz = Configuracao::TipoCruzamento::pmx; break;
	case 2: cruz = Configuracao::TipoCruzamento::ciclo; break;
	case 3: cruz = Configuracao::TipoCruzamento::ordem; break;
	default: goto err;
	}

	switch (optgrade) {
	case 1: grade = Configuracao::TipoGrade::modelo; break;
	case 2: grade = Configuracao::TipoGrade::grasp; break;
	default: goto err;
	}

	dados << "Individuos: " << numindividuos << "\n";
	dados << "Numero maximo iteracoes (AG) " << numiterag << "\n";
	dados << "Numero maximo iteracoes (GRASP) " << numitergrasp << "\n";
	dados << "Opt grade: " << optgrade << "\n";
	dados << "Opt cruz: " << optcruz << "\n";

	for (auto i = 0; i < num; i++) {
		exper(filein, mut, cruz, grade, numindividuos, percent_cruz, numiterag,
			  numitergrasp);
	}
	return;

err:
	std::cout << "Opcao invalida, saindo\n";
	return;
}

void novo_experimento_cli(const std::string& input, const std::string& file)
{
	std::ifstream config{file};

	// comentário do topo
	// formato entrada: 
	// ID AGIter NIndiv %Cruz NMut NTour GRASPIter GRASPNVzi GRASPAlfa NExec
	// formato saida:
	// ID,NExec,Tempo,Fo

	std::string id;
	int ag_iter, n_indiv, p_cruz, n_mut, n_tour, grasp_iter;
	int grasp_nviz, grasp_alfa, n_exec;
	
	while (config >> id >> ag_iter >> n_indiv >> p_cruz >> n_mut >> n_tour 
			   >> grasp_iter >> grasp_nviz >> grasp_alfa >> n_exec) {

		auto path = Util::join_path({"exper", "result"});
		Util::create_folder(path);

		auto filename = path + "result" + id + ".txt";
		std::ofstream out{filename};

		std::cout << "\n\nID: " << id << "\n";

		out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";
		for (auto i = 0; i < n_exec; i++) {
			long long tempo; double fo;
			std::tie(tempo, fo) = novo_experimento(
				input, id, n_indiv, p_cruz, grasp_iter, ag_iter,
				grasp_nviz, grasp_alfa, n_tour, n_mut, i);

			std::cout << id << "," << i << "," << tempo << "," << fo << "\n";
			out << id << "," << i << "," << tempo << "," << fo << "\n";
			
			// TODO:
			// Refatorar os métodos de crossover (DRY)
		}
	}

}

void novo_experimento_cli_fase2(const std::string& input, const std::string& file)
{
	experimento = true;
	std::ifstream config{file};

	// comentário do topo
	// formato entrada: 
	// ID AGIter NIndiv %Cruz NMut NTour GRASPIter GRASPNVzi GRASPAlfa NExec
	// formato saida:
	// ID,NExec,Tempo,Fo

	std::string id;
	int ag_iter, xover_id, n_exec;
	Configuracao::TipoCruzamento xover;

	while (config >> id >> ag_iter >> xover_id >> n_exec) {
		switch (xover_id) {
		case 1:
			xover = Configuracao::TipoCruzamento::pmx; break;
		case 2:
			xover = Configuracao::TipoCruzamento::ordem; break;
		case 3:
			xover = Configuracao::TipoCruzamento::ciclo; break;
		default:
			xover = Configuracao::TipoCruzamento::pmx; break;
		}

		auto path = Util::join_path({"exper", "result"});
		Util::create_folder(path);

		auto filename = path + "result" + id + ".txt";
		std::ofstream out{filename};

		std::cout << "\n\nID: " << id << "\n";

		out << "ID Algoritmo, Numero execucao, Tempo total, FO\n";
		for (auto i = 0; i < n_exec; i++) {
			long long tempo; double fo;
			std::tie(tempo, fo) = novo_experimento_fase2(
				input, id, i, ag_iter, xover);

			std::cout << id << "," << i << "," << tempo << "," << fo << "\n";
			out << id << "," << i << "," << tempo << "," << fo << "\n";

			// TODO:
			// Refatorar os métodos de crossover (DRY)
		}
	}

}

int main(int argc, char** argv)
{
	verbose = false;
	if (argc == 3) {
		// Primeiro argumento é a entrada, o segundo é o arquivo de configuração
		novo_experimento_cli_fase2(argv[1], argv[2]);
	} else if (argc == 2) {
		//calibracao();
		//menu(argv[1]);
		std::string flag = argv[1];
		if (flag == "-h" || flag == "--help") {
			std::cout << "Primeiro argumento é a entrada, o segundo é o arquivo de configuração\n";
			std::cout << "Formato da config:\n";
			std::cout << "ID AGIter NIndiv %Cruz NMut NTour GRASPIter GRASPNVzi GRASPAlfa NExec\n";
		}
	} else {
		semArgumentos();
		//teste();
	}
}

