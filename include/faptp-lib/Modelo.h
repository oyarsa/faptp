#pragma once

#include <faptp-lib/DadosModelo.h>
#include <json/json.h>

#ifdef GUROBI_ENABLED
Json::Value modelo_gurobi(const DadosModelo& dados);
#endif

#ifdef CPLEX_ENABLED
Json::Value modelo_cplex(const DadosModelo& dados);
#endif

Json::Value modelo(const DadosModelo& dados)
{
#ifdef GUROBI_ENABLED
  return modelo_gurobi(dados);
#elif CPLEX_ENABLED
  return modelo_cplex(dados);
#else
  throw std::runtime_error{ "faptp deve ser compilado com Gurobi ou CPLEX para \
    suportar Json::Value modelo(const DadosModelo&)" };
#endif
}