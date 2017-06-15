#include <faptp-lib/Modelo.h>
#include <algorithm>
#include <limits>
#include <numeric>
#include <gurobi_c++.h>

#ifdef CPLEX_ENABLED
Json::Value 
modelo_cplex(const DadosModelo& dados)
{
  return {};
}
#endif
