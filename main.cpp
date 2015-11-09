#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <sstream>
#include <vector>

#include "template/Algorithms.h"

#include "src/includes/parametros.h"

#include "src/Disciplina.h"
#include "src/Professor.h"
#include "src/Horario.h"
#include "src/Util.h"
#include "src/Resolucao.h"

int main(int argc, char** argv) {
    verbose = false;

    // Resolucao *resolucaoGuloso, *resolucaoGrasp, *resolucaoCombinatorio;

    //  resolucaoGuloso = new Resolucao(3, 2, 5, TXT_SOLUCAO);
    //  std::cout << "-GULOSO-" << std::endl;
    //  resolucaoGuloso->start(RESOLUCAO_GERAR_GRADE_TIPO_GULOSO);
    //  delete resolucaoGuloso;

    // Inicializa um objeto resolução com blocos tamanho 3, 2 camadas e 
    // 5 perfis de alunos
    
    Resolucao resolucaoGrasp(3, 2, 5);
    if (verbose) {
        std::cout << "\n\n\n\n--------------------\n\n\n\n";
        std::cout << "-GRASP-" << std::endl;
    }

    auto inicio = clock();
    
    // Escolhe o algoritmo de geração de vizinhos como aleatórios, e o número
    // de vizinhos gerados como 4 por iteração, e o limite de parada como 0.8ms
    resolucaoGrasp.gradeGraspVizinhanca = RESOLUCAO_GRASP_VIZINHOS_ALEATORIOS;
    
    resolucaoGrasp.gradeGraspVizinhos = 4;
    resolucaoGrasp.gradeGraspTempoConstrucao = .8;
    
    resolucaoGrasp.gradeTipoConstrucao = RESOLUCAO_GERAR_GRADE_TIPO_GRASP;
    resolucaoGrasp.gradeAlfa = .9;

    // Inicia a execução do algoritmo
    resolucaoGrasp.start();

    /**
     * TODO: arrumar gerador combinatório
     */

    //  resolucaoCombinatorio = new Resolucao(3, 2, 5, TXT_SOLUCAO);
    //  std::cout << "-COMBINATORIO-" << std::endl;
    //  resolucaoCombinatorio->start(RESOLUCAO_GERAR_GRADE_TIPO_COMBINATORIO, 0);
    //  delete resolucaoCombinatorio;
    
    auto fim = clock();
    
    std::cout << "Tempo: " << fim - inicio << "\n";
}
