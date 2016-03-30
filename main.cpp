#include <algorithm>
#include <iostream>
#include <fstream>
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
		//.arquivoEntrada(Util::join_path({"res"}, "input_gigante.json"))
		.arquivoEntrada(Util::join_path({"res"}, "input_gigante2.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input_maroto3.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input.json"))
		.populacaoInicial(100)
		.porcentagemCruzamentos(40) // %
		.numMaximoIteracoesSemEvolucaoGRASP(20)
		.numMaximoIteracoesSemEvolucaoAG(100)
		//.tipoCruzamento(Configuracao::TipoCruzamento::pmx)
		//.tipoCruzamento(Configuracao::TipoCruzamento::substitui_bloco)
		//.tipoCruzamento(Configuracao::TipoCruzamento::simples)
		//.tipoCruzamento(Configuracao::TipoCruzamento::construtivo_reparo)
		//.tipoCruzamento(Configuracao::TipoCruzamento::ciclo)
		//.tipoCruzamento(Configuracao::TipoCruzamento::ordem)
		//.tipoMutacao(Configuracao::TipoMutacao::substitui_professor)
		.tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
		.mutacaoProbabilidade(20) // %
		.graspNumVizinhos(2)
		.graspAlfa(40) // %
		.camadaTamanho(20)
		.perfilTamanho(600)
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

	std::ofstream out{savePath + "teste.txt"};
	r.logExperimentos();
	out << oss.str();
	out << r.getLog();
}


void exper(const std::string& filein, 
		 Configuracao::TipoMutacao mut, Configuracao::TipoCruzamento cruz,
		 Configuracao::TipoGrade grade, int numindividuos, int numger)
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
		.graspTempoConstrucao(30000) // ms
		.graspNumVizinhos(2)
		.graspAlfa(30) // %
		.numIteracoes(numger)
		.numTorneioPares(0)
		.numTorneioPopulacao(1)
		.tentativasMutacao(2)
		.graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
		.tipoConstrucao(grade)
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

void teste()
{
	Resolucao resolucaoGrasp{Configuracao()
		//.arquivoEntrada(Util::join_path({"res"}, "input_gigante.json"))
		.arquivoEntrada(Util::join_path({"res"}, "input_gigante2.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input_maroto3.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input.json"))
		.populacaoInicial(1)
		.numIteracoes(0)
		.porcentagemSolucoesAleatorias(0) // %
		.porcentagemCruzamentos(0) // %
		.numMaximoIteracoesSemEvolucaoGRASP(20)
		.tipoCruzamento(Configuracao::TipoCruzamento::substitui_bloco)
		//.tipoCruzamento(Configuracao::TipoCruzamento::simples)
		//.tipoCruzamento(Configuracao::TipoCruzamento::construtivo_reparo)
		//.tipoMutacao(Configuracao::TipoMutacao::substitui_professor)
		.tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
		.mutacaoProbabilidade(20) // %
		.graspTempoConstrucao(600) // ms
		.graspNumVizinhos(2)
		.graspAlfa(30) // %
		.camadaTamanho(20)
		.perfilTamanho(600)
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

	Configuracao::TipoMutacao mut;
	Configuracao::TipoGrade grade;
	Configuracao::TipoCruzamento cruz;
	
	std::cout << "\n";

	int optcruz;
	std::cout << "Operador de cruzamento:\n";
	std::cout << "1 - Construtivo-reparo (original)\n";
	std::cout << "2 - Simples (troca-camada)\n";
	std::cout << "3 - Troca-blocos\n";
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

	int optmut;
	std::cout << "Operador de mutacao:\n";
	std::cout << "1 - Substitui disciplina\n";
	std::cout << "2 - Substitui professor\n";
	std::cout << "Opcao> ";
	std::cin >> optmut;

	std::cout << "\n";

	int numindividuos, numger;
	std::cout << "Quantos individuos? ";
	std::cin >> numindividuos;
	std::cout << "Quantas geracoes? ";
	std::cin >> numger;

	std::cout << "\n\n";

	switch (optcruz) {
	case 1: cruz = Configuracao::TipoCruzamento::construtivo_reparo; break;
	case 2: cruz = Configuracao::TipoCruzamento::simples; break;
	case 3: cruz = Configuracao::TipoCruzamento::substitui_bloco; break;
	default: goto err;
	}

	switch (optgrade) {
	case 1: grade = Configuracao::TipoGrade::modelo; break;
	case 2: grade = Configuracao::TipoGrade::grasp; break;
	default: goto err;
	}

	switch (optmut) {
	case 1: mut = Configuracao::TipoMutacao::substiui_disciplina; break;
	case 2: mut = Configuracao::TipoMutacao::substitui_professor; break;
	default: goto err;
	}

	dados << "Individuos: " << numindividuos << "\n";
	dados << "Geracoes: " << numger << "\n";
	dados << "Opt grade: " << optgrade << "\n";
	dados << "Opt mut: " << optmut << "\n";
	dados << "Opt cruz: " << optcruz << "\n";

	for (auto i = 0; i < num; i++) {
		exper(filein, mut, cruz, grade, numindividuos, numger);
	}
	return;

err:
	std::cout << "Opcao invalida, saindo\n";
	return;
}

int main(int argc, char** argv)
{
	verbose = false;
	if (argc == 3) {
		comArgumentos(argv);
	} else if (argc == 2) {
		//calibracao();
		menu(argv[1]);
	} else {
		semArgumentos();
		//teste();
	}
}

