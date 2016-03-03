#include <sstream>

#include "Util.h"
#include "Aleatorio.h"

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
  
  triDimensional[1] = x; // Dia
  triDimensional[0] = y; // Bloco
  triDimensional[2] = z; // Camada
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

long long Util::chronoDiff(std::chrono::time_point<std::chrono::system_clock> t1, std::chrono::time_point<std::chrono::system_clock> t2)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>
		(t1 - t2).count();
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
    random = (aleatorio::randomInt() % (max - min)) + min;
  }
  return random;
}

std::string Util::join_path(const std::vector<std::string>& folders, const std::string& file)
{
	auto oss = std::ostringstream{};
#if defined(_WIN32)
	auto sep = "\\";
#else
	auto sep = "/";
#endif
	for (const auto& f : folders) {
		oss << f << sep;
	}
	oss << file;

	return oss.str();
}

void Util::create_folder(const std::string& path)
{
#if defined(_WIN32)
    std::string command {"mkdir " + path + " 2> NUL"};
#else
	std::string command {"mkdir -p" + path + " > /dev/null 2>&1"};
#endif
    system(command.c_str());
}
