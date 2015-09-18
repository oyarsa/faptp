
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "template/Algorithms.h"

#include "src/Disciplina.h"
#include "src/Professor.h"
#include "src/Horario.h"
#include "src/Util.h"
#include "src/Resolucao.h"

void separator() {
  std::cout << "\n\n\n\n--------------------\n\n\n\n";
}

int main(int argc, char** argv) {
  Resolucao *resolucaoGuloso, *resolucaoGrasp, *resolucaoCombinatorio;
  
  resolucaoGuloso = new Resolucao(3, 2, 5, TXT_SOLUCAO);
  std::cout << "-GULOSO-" << std::endl;
  resolucaoGuloso->start(RESOLUCAO_GERAR_GRADE_TIPO_GULOSO);
  delete resolucaoGuloso;
  
  resolucaoGrasp = new Resolucao(3, 2, 5, TXT_SOLUCAO);
  separator();
  std::cout << "-GRASP-" << std::endl;
  
  resolucaoGrasp->graspVizinhanca = RESOLUCAO_GRASP_VIZINHOS_ALEATORIOS;
  resolucaoGrasp->graspVizinhos = 10;
  resolucaoGrasp->graspTempoConstrucao = 1.8;
  
  resolucaoGrasp->start(RESOLUCAO_GERAR_GRADE_TIPO_GRASP, 0.9);
  delete resolucaoGrasp;
  
    /**
     * TODO: arrumar gerador combinatório
     */
//  resolucaoCombinatorio = new Resolucao(3, 2, 5, TXT_SOLUCAO);
//  separator();
//  std::cout << "-COMBINATORIO-" << std::endl;
//  resolucaoCombinatorio->start(RESOLUCAO_GERAR_GRADE_TIPO_COMBINATORIO, 0);
//  delete resolucaoCombinatorio;


  return EXIT_SUCCESS;
}
