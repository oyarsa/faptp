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
    
    auto inicio = clock();
    
    Resolucao resolucaoGrasp(3, 2, 5, TXT_SOLUCAO);
    if (verbose) {
        std::cout << "\n\n\n\n--------------------\n\n\n\n";
        std::cout << "-GRASP-" << std::endl;
    }

    // Escolhe o algoritmo de geração de vizinhos como aleatórios, e o número
    // de vizinhos gerados como 10 por iteração, e o limite de parada como 1.8ms
    resolucaoGrasp.graspVizinhanca = RESOLUCAO_GRASP_VIZINHOS_ALEATORIOS;
    
    
    resolucaoGrasp.graspVizinhos = 4;
    resolucaoGrasp.graspTempoConstrucao = 0.8;

    // Inicia a execução do algoritmo
    resolucaoGrasp.start(RESOLUCAO_GERAR_GRADE_TIPO_GRASP, 0.9);

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
