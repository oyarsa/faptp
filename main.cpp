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
	Output o;

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
			o.write(resolucaoGrasp.getSolucao(), savePath);
		}
	}
}

void semArgumentos()
{
	Output o;

	experimento = false;

	Resolucao resolucaoGrasp {Configuracao()
		.arquivoEntrada("input_fagoc.json")
		.populacaoInicial(10)
		.mutacaoProbabilidade(20) // %
		.graspTempoConstrucao(1) // ms
		.graspNumVizinhos(2)
		.graspAlfa(30) // %
		.numIteracoes(10)
		.numTorneioPares(0)
		.numTorneioPopulacao(1)
		.tentativasMutacao(2)
		.graspVizinhanca(Configuracao::TipoVizinhos::aleatorios)
		.tipoConstrucao(Configuracao::TipoGrade::modelo)};

	std::cout << "Montando horarios [AG + Modelo]..." << std::endl;

	auto inicio= std::chrono::steady_clock::now();
	resolucaoGrasp.start(false);
	auto fim = std::chrono::steady_clock::now();

	std::cout << "Tempo do horario: " << std::chrono::duration_cast<std::chrono::milliseconds>
		(fim - inicio).count() << "ms\n\n";

	auto fo = resolucaoGrasp.getSolucao()->getObjectiveFunction();
	std::cout << "\nResultado:" << fo << std::endl;
	resolucaoGrasp.showResult();

#if defined(_WIN32)
	std::string folder{"teste\\"};
#else
	std::string folder{"teste/"};
#endif
	auto savePath = folder + "fo" + std::to_string(fo);
	o.write(resolucaoGrasp.getSolucao(), savePath);
}

int main(int argc, char** argv)
{
	verbose = false;

	if (argc == 3) {
		comArgumentos(argv);
	} else if (argc == 2) {
		calibracao();
	} else {
		semArgumentos();
	}
}

