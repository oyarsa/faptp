#include <faptp-lib/Modelo.h>
#include <algorithm>
#include <limits>
#include <numeric>
#include <gurobi_c++.h>

namespace {

template <typename T>
using Vec = std::vector<T>;

template <typename T>
using Vec2D = Vec<Vec<T>>;

template <typename T>
using Vec3D = Vec<Vec<Vec<T>>>;

template <typename T>
using Vec4D = Vec<Vec<Vec<Vec<T>>>>;

template <typename T, typename Func>
Vec<T> vec(std::size_t x, Func init)
{ 
  auto v = Vec<T>(x);
  std::generate(begin(v), end(v), init);
}

template <typename T, typename Func>
Vec2D<T> vec(std::size_t x, std::size_t y, Func init)
{ 
  return Vec2D<T>(x, vec<T>(y, init));
}

template <typename T, typename Func>
Vec3D<T> vec(std::size_t x, std::size_t y, std::size_t z, Func init)
{ 
  return Vec3D<T>(x, vec<T>(y, z, init));
}

template <typename T, typename Func>
Vec4D<T> vec(std::size_t w, std::size_t x, std::size_t y, 
                  std::size_t z, Func init)
{ 
  return Vec4D<T>(w, vec<T>(x, y, z, init));
}

template <typename Container>
GRBLinExpr sum(const Container& c)
{ 
  return std::accumulate(begin(c), end(c), GRBLinExpr(0.0));
}

template <typename Container2D>
GRBLinExpr sum2D(const Container2D& c)
{ 
  return std::accumulate(
    begin(c), end(c), GRBLinExpr(0.0),
    [](auto acc, const auto& el) { return acc + sum(el); });
}

constexpr auto inf = std::numeric_limits<double>::infinity();

}

namespace gurobi {

GRBEnv make_env()
{
  return GRBEnv{ };
}

GRBModel make_model(const GRBEnv& env)
{
  return GRBModel{ env };
}

GRBVar add_bin_var(GRBModel& model)
{
  return model.addVar(0, 1, 0, GRB_BINARY);
}

GRBVar add_int_var(GRBModel& model)
{
  return model.addVar(0, inf, 0, GRB_INTEGER);
}

void set_min_objective(GRBModel& model, GRBLinExpr obj)
{
  model.setObjective(obj, GRB_MINIMIZE);
}

void add_constraint(GRBModel& model, GRBTempConstr& constr)
{
  model.addConstr(constr);
}

template<typename... Args>
inline auto dvar(Args&&... args)->decltype(vec<GRBVar>(std::forward<Args>(args)...))
{
  return vec<GRBVar>(std::forward<Args>(args)...);
}

}


// TODO: Mover funções genéricas para uma classe static
// e usar a classe como parâmetro do template para o modelo
// (a la numeric_limits<T>)

#ifdef GUROBI_ENABLED
Json::Value 
modelo_gurobi(const DadosModelo& dados)
{
  using namespace gurobi; 

  const auto C = dados.num_camadas();
  const auto P = dados.num_professores();
  const auto D = dados.num_disciplinas();
  const auto I = dados.num_horarios();
  const auto J = dados.num_dias();
  const auto Ja = dados.num_horarios() - 2;
  const auto Jb = dados.num_dias() - 2;
  const auto pi = dados.pesos_constraints();

  auto env = make_env();
  auto model = make_model(env);

  const auto bin_var = [&model]() { return add_bin_var(model); };
  const auto int_var = [&model]() { return add_int_var(model); };

  auto x = dvar(P, D, I, bin_var);
  auto r = dvar(C, I, J, int_var);
  auto alfa1 = dvar(Ja, C, I, J, bin_var);
  auto alfa = dvar(C, int_var);
  auto w = dvar(P, J, bin_var);
  auto beta1 = dvar(Jb, P, J, bin_var);
  auto beta = dvar(P, int_var);
  auto g = dvar(C, J, bin_var);
  auto gama = dvar(C, int_var);
  auto delta = dvar(C, int_var);
  auto epsilon = dvar(D, J, int_var);
  auto teta = dvar(J, C, int_var);
  auto capa = dvar(C, J, bin_var);
  auto lambda = dvar(P, int_var);
  auto mi = dvar(P, int_var);
  auto Lec = dvar(P, D, bin_var);
  auto gem = dvar(P, D, I, J, bin_var);

  auto janelas = sum(alfa);
  auto intervalos = sum(beta);
  auto compacto = sum(gama);
  auto sabados = sum(delta);
  auto seguidas = sum2D(epsilon);
  auto dificeis_seguidas = sum2D(teta);
  auto dificeis_ultimo = sum2D(capa);
  auto pref_disc = sum(lambda);
  auto pref_aula = sum(mi);

  auto objetivo =
    pi[0] * janelas +
    pi[1] * intervalos +
    pi[2] * compacto +
    pi[3] * sabados +
    pi[4] * seguidas +
    pi[5] * dificeis_seguidas +
    pi[6] * dificeis_ultimo +
    pi[7] * pref_disc +
    pi[8] * pref_aula;

  set_min_objective(model, objetivo);

  return {};
}
#endif

#ifdef CPLEX_ENABLED
Json::Value 
modelo_cplex(const DadosModelo& dados)
{
  return {};
}
#endif
