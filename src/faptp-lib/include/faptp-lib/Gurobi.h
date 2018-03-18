#pragma once

#include <limits>
#include <gurobi_c++.h>

struct Gurobi {
  using Env_t = GRBEnv;
  using Model_t = GRBModel;
  using BinVar_t = GRBVar;
  using IntVar_t = GRBVar;
  using Expr_t = GRBLinExpr;
  using Constr_t = GRBTempConstr;

  static Env_t 
  make_env()
  {
    return GRBEnv{ };
  }

  static Model_t 
  make_model(const Env_t& env)
  {
    return GRBModel{ env };
  }

  static BinVar_t 
  add_bin_var(Model_t& model)
  {
    return model.addVar(0, 1, 0, GRB_BINARY);
  }

  static IntVar_t 
  add_int_var(Model_t& model)
  {
    auto inf = std::numeric_limits<double>::infinity();
    return model.addVar(0, inf, 0, GRB_INTEGER);
  }

  static void 
  set_min_objective(Model_t& model, Expr_t obj)
  {
    model.setObjective(obj, GRB_MINIMIZE);
  }

  static void 
  add_constraint(Model_t& model, const Constr_t& constr)
  {
    model.addConstr(constr);
  }
};