#include "Util.h"
#include "includes/parametros.h"

Util::Util() {
  srand(time(NULL));
}

Util::~Util() {
}

int Util::getPosition(int y, int x, int z, int Y, int Z) {
  return (x + (y * Y) + (z * Y * Z));
}

void Util::get3DMatrix(int pLinear, int* triDimensional, int X, int Y, int Z) {
  int x, y, z;
  int tamanho = ((X * Y) * Z);

  // Dia
  x = ((pLinear % tamanho) / Y) % X;
  // Bloco
  y = ((pLinear % tamanho) % Y);
  // Camada
  z = pLinear / (X * Y);
  
  triDimensional[0] = y;
  triDimensional[1] = x;
  triDimensional[2] = z;
}

std::vector<std::string> &Util::strSplit(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> Util::strSplit(const std::string &s, char delim) {
  std::vector<std::string> elems;
  strSplit(s, delim, elems);
  return elems;
}

double Util::timeDiff(clock_t tf, clock_t t0) {
  return ((((float) tf - (float) t0) / 1000000.0F) * 1000);
}

int Util::randomBetween(int min, int max) {
  int random = 0;

  if (max < min) {
    random = -1;
    //std::cerr << "The min number is biggest than max";
  } else if (max == min) {
    random = max;
  } else if (max != 0) {
//    random = (rand() % (max - min)) + min;
    random = (aleatorio.randomInt() % (max - min)) + min;
  }
  return random;
}
