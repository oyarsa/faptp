#ifndef UTIL_H
#define	UTIL_H

#include <string>
#include <sstream>
#include <vector>

class Util {
public:
  Util();
  virtual ~Util();
  
  int getPosition(int y, int x, int z, int Y, int Z);
  void get3DMatrix(int pLinear, int* triDimensional, int X, int Y, int Z);
  
  std::vector<std::string> &strSplit(const std::string &s, char delim, std::vector<std::string> &elems);
  std::vector<std::string> strSplit(const std::string &s, char delim);
  
};

#endif	/* UTIL_H */

