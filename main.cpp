
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

int getPosition(int y, int x, int z, int Y, int Z) {
  return (x + (y * Y) + (z * Y * Z));
}

void print3DMatrix(int pLinear, int Y, int Z) {
  int x, y, z;

  z = pLinear / (Y * Z);
  pLinear -= z * (Y * Z);

  y = pLinear / Y;
  pLinear -= y * Y;

  x = pLinear / 1;

  std::cout << "Y[line]: " << y << std::endl;
  std::cout << "X[col] : " << x << std::endl;
  std::cout << "Z[prof]: " << z << std::endl;
}

int main(int argc, char** argv) {
  /*
  UUID uuid;

  Professor *p = new Professor("Pedro Henrique M�zala");
  Horario *h = new Horario(4, 4);

  std::string testeStr = "teste";
  char testeChar[] = "teste\0";
  testeStr = testeChar;
  
  std::vector<int> testeInt;
  testeInt.resize(10);
  testeInt[0] = 1;
  testeInt[1] = 1;
  testeInt[9] = 1;

  std::cout << p->getNome() << std::endl;
  std::cout << p->getId() << std::endl;
  std::cout << "[" << (testeStr == testeChar) << "]" << std::endl;
  std::cout << "[" << testeInt.size()  << "]" << std::endl;
  std::cout << "[" << uuid.GenerateUuid()  << "]" << std::endl;
   */

  /*
  Util util;
  std::vector<std::string> pieces;
  std::string line;
  std::ifstream myfile("input/Disciplina.txt");
  
  if (myfile.is_open()) {
    while (std::getline(myfile, line)) {
      pieces = util.strSplit(line, ';');
      for (int i = 0; i < pieces.size(); i++) {
        if (pieces[i] != "\n")
          std::cout << pieces[i] << "][";
      }
      std::cout << std::endl;
    }
    myfile.close();
  } else {
    std::cout << "Unable to open file";
    exit(EXIT_FAILURE);
  }
   */
  /*
  Util util;
  clock_t t0 = clock();
  while (util.timeDiff(clock(), t0) <= 700);
  return 0;
   */
  /*
  std::vector<int>teste;
  teste.push_back(0);
  teste.push_back(1);
  teste.push_back(2);
  teste.push_back(3);
  teste.push_back(4);
  std::vector<int>::iterator it = teste.begin();
  srand (time(NULL));
  std::cout << (rand()%(5-0))+0 << std::endl;
  std::cout << (rand()%(5-0))+0 << std::endl;
  std::cout << (rand()%(5-0))+0 << std::endl;
  return 0;
   */
  /*
  std::vector<int> teste;
  teste.push_back(-1);
  teste.push_back(1);
  teste.push_back(2);
  teste.push_back(3);
  teste.push_back(4);
  teste.push_back(5);
  std::cout << teste.size() << std::endl;
  std::cout << "Remove  e: " << *(std::remove(teste.begin(), teste.end(), 2)) << std::endl;
  teste.erase(std::remove(teste.begin(), teste.end(), 2), teste.end());
  std::cout << teste[2] << std::endl;
  std::cout << teste.size() << std::endl;
  std::cout << "Remove ne: " << (std::remove(teste.begin(), teste.end(), 2) == teste.end() ? -500 : *(std::remove(teste.begin(), teste.end(), 2))) << std::endl;
  teste.erase(std::remove(teste.begin(), teste.end(), 2), teste.end());
  std::cout << teste[2] << std::endl;
  std::cout << teste.size() << std::endl;
  return 0;
   */

  Resolucao *resolucao = new Resolucao(3, 2, 5, TXT_SOLUCAO);
  resolucao->start(RESOLUCAO_GERAR_GRADE_TIPO_COMBINATORIO, .8);
  delete resolucao;

  return EXIT_SUCCESS;
}