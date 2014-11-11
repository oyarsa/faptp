#ifndef UTIL_H
#define	UTIL_H

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>

#include <time.h>

class Util {
public:
  Util();
  virtual ~Util();
  
  int getPosition(int y, int x, int z, int Y, int Z);
  void get3DMatrix(int pLinear, int* triDimensional, int X, int Y, int Z);
  
  std::vector<std::string> &strSplit(const std::string &s, char delim, std::vector<std::string> &elems);
  std::vector<std::string> strSplit(const std::string &s, char delim);
  
  double timeDiff(clock_t tf, clock_t t0);
  
  int randomBetween(int min, int max);
  
};

#endif	/* UTIL_H */

