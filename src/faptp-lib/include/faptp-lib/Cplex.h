#pragma once

struct Gurobi {
  using Env_t = ?;
  using Model_t = ?;
  using Var_t = ?;
  using Expr_t = ?;
  using Constr_t = ?;

  static Env_t
  make_env()
  {
  }

  static Model_t
  make_model(const Env_t& env)
  {
  }

  static Var_t
  add_bin_var(Model_t& model)
  {
  }

  static Var_t
  add_int_var(Model_t& model)
  {
  }

  static void
  set_min_objective(Model_t& model, Expr_t obj)
  {
  }

  static void
  add_constraint(Model_t& model, const Constr_t& constr)
  {
  }
};