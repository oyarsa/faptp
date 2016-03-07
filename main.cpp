#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <chrono>

#include <modelo-grade/modelo_solver.h>
#include <modelo-grade/arquivos.h>

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

			fo = resolucaoGrasp.getSolucao()->getObjectiveFunction();
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

	Resolucao resolucaoGrasp {Configuracao()
		.arquivoEntrada(Util::join_path({"res"}, "input_maroto3.json"))
		//.arquivoEntrada(Util::join_path({"res"}, "input.json"))
		.populacaoInicial(10)
		.porcentagemCruzamentos(20) // %
		.tipoCruzamento(Configuracao::TipoCruzamento::substitui_bloco)
		//.tipoCruzamento(Configuracao::TipoCruzamento::simples)
		//.tipoCruzamento(Configuracao::TipoCruzamento::construtivo_reparo)
		//.tipoMutacao(Configuracao::TipoMutacao::substitui_professor)
		.tipoMutacao(Configuracao::TipoMutacao::substiui_disciplina)
		.mutacaoProbabilidade(20) // %
		.graspTempoConstrucao(1) // ms
		.graspNumVizinhos(2)
		.graspAlfa(30) // %
		.porcentagemSolucoesAleatorias(0) // %
		.numIteracoes(5)
		.numTorneioPares(0)
		.numTorneioPopulacao(1)
		.tentativasMutacao(2)
		.graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
		//.tipoConstrucao(Configuracao::TipoGrade::modelo)
		.tipoConstrucao(Configuracao::TipoGrade::grasp)
	};

	std::cout << "Montando horarios [AG + Modelo]..." << std::endl;

	auto inicio = std::chrono::steady_clock::now();
	resolucaoGrasp.start(false);
	auto fim = std::chrono::steady_clock::now();

	std::cout << "Tempo do horario: " << std::chrono::duration_cast<std::chrono::milliseconds>
		(fim - inicio).count() << "ms\n\n";

	auto fo = resolucaoGrasp.getSolucao()->getObjectiveFunction();
	std::cout << "\nResultado:" << fo << std::endl;
	//resolucaoGrasp.showResult();

	auto savePath = Util::join_path({"teste", "fo" + std::to_string(fo)});
	Output::write(resolucaoGrasp.getSolucao(), savePath);

	std::cout << "solucaoAlvo: " << resolucaoGrasp.foAlvo << "\n";
	std::cout << "hashAlvo: " << resolucaoGrasp.hashAlvo << "\n";
	std::cout << "iteracoes: " << resolucaoGrasp.iteracaoAlvo << "\n";
	std::cout << "tempoAlvo: " << resolucaoGrasp.tempoAlvo << "\n";
}


void exper(const std::string& filein, const std::string& fileout,
		 Configuracao::TipoMutacao mut, Configuracao::TipoCruzamento cruz,
		 Configuracao::TipoGrade grade)
{
	experimento = false;

	Resolucao resolucaoGrasp{Configuracao()
		.arquivoEntrada(filein)
		.populacaoInicial(100)
		.porcentagemCruzamentos(0) // %
		.tipoCruzamento(cruz)
		.tipoMutacao(mut)
		.mutacaoProbabilidade(20) // %
		.graspTempoConstrucao(1000) // ms
		.graspNumVizinhos(2)
		.graspAlfa(30) // %
		.numIteracoes(100)
		.numTorneioPares(0)
		.numTorneioPopulacao(1)
		.tentativasMutacao(2)
		.graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
		.tipoConstrucao(grade)
	};

	auto inicio = std::chrono::steady_clock::now();
	resolucaoGrasp.start(false);
	auto fim = std::chrono::steady_clock::now();

	auto saida = std::ofstream(fileout);
	if (!saida.is_open()) std::cout << "Ixi\n";
	saida << "Tempo do horario: " << std::chrono::duration_cast<std::chrono::milliseconds>
		(fim - inicio).count() << "ms\n\n";

	auto fo = resolucaoGrasp.getSolucao()->getObjectiveFunction();
	saida << "\nResultado:" << fo << std::endl;
	//resolucaoGrasp.showResult();

	saida << "solucaoAlvo:" << resolucaoGrasp.foAlvo << "\n";
	saida << "iteracoes:" << resolucaoGrasp.iteracaoAlvo << "\n";
	saida << "tempoAlvo:" << resolucaoGrasp.tempoAlvo << "\n";
}

void menu(std::string filein)
{
	int num;
	std::cout << "Rodar quantas vezes? ";
	std::cin >> num;

	auto fileout = Output::timestamp() + ".txt";
	std::cout << "File: " << fileout << "\n";
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

	exper(filein, fileout, mut, cruz, grade);
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
	}
}

