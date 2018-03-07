#pragma once

#include <faptp-lib/DadosModelo.h>
#include <numeric>
#include <json/json.h>
#include <range/range.hpp>

#ifdef GUROBI_ENABLED
  #include <faptp-lib/Gurobi.h>
#endif

namespace detail {

template <typename T>
using Vec = std::vector<T>;

template <typename T>
using Vec2D = Vec<Vec<T>>;

template <typename T>
using Vec3D = Vec<Vec<Vec<T>>>;

template <typename T>
using Vec4D = Vec<Vec<Vec<Vec<T>>>>;

template <typename Func>
auto vec(std::size_t x, Func init)
{ 
  using T = decltype(init());
  auto v = Vec<T>(x);
  std::generate(begin(v), end(v), init);
  return v;
}

template <typename Func>
auto vec(std::size_t x, std::size_t y, Func init)
{ 
  using T = decltype(init());
  return Vec2D<T>(x, vec(y, init));
}

template <typename Func>
auto vec(std::size_t x, std::size_t y, std::size_t z, Func init)
{ 
  using T = decltype(init());
  return Vec3D<T>(x, vec(y, z, init));
}

template <typename Func>
auto vec(std::size_t w, std::size_t x, std::size_t y, 
                  std::size_t z, Func init)
{ 
  using T = decltype(init());
  return Vec4D<T>(w, vec(x, y, z, init));
}

constexpr auto inf = std::numeric_limits<double>::infinity();

template <typename Solver, typename Container>
typename Solver::Expr_t sum(const Container& c)
{ 
  return std::accumulate(begin(c), end(c), typename Solver::Expr_t(0.0));
}

template <typename Solver, typename Container2D>
typename Solver::Expr_t sum2D(const Container2D& c)
{ 
  return std::accumulate(begin(c), end(c), typename Solver::Expr_t(0.0), 
    [](auto acc, const auto& el) { return acc + sum<Solver>(el); });
}

} // namespace detail


/**
  concept Solver

  Tipos associados:
    Env_t: contexto do Solver. Ex. IloEnv, GRBEnv.
    Model_t: objeto do modelo, criado a partir do contexto.
    BinVar_t: variável de decisão binária.
    IntVar_t: variável de decisão inteira.
    Expr_t: expressão, composta por constantes e variáveis de decisão
            associadas por operadores.
    Constr_t: restrição, formada por expressões e operadores de comparação.

  Funções:
    Env_t make_env()
      Inicializa o solver.

    Model_t make_model(const Env_t& env)
      Cria um novo modelo a partir do contexto.

    BinVar_t add_bin_var(Model_t& model)
      Cria uma nova variável de decisão binária.

    IntVar_t add_int_var(Model_t& model)
      Cria uma nova variável de decisão inteira.

    void set_min_objective(Model_t& model, Expr_t obj)
      Configura o objetivo do modelo como minimização.

    void add_constraint(Model_t& const Constr_t& constr)
      Adiciona uma restrição ao modelo.
*/

template <typename Solver>
Json::Value 
solve(const DadosModelo& dados)
{
  using namespace detail;
  using range = Range::basic_range<std::size_t>;

  //    Dados de entrada
  // Quantidades
  const auto C = dados.num_camadas();
  const auto P = dados.num_professores();
  const auto D = dados.num_disciplinas();
  const auto I = dados.num_horarios();
  const auto J = dados.num_dias();
  const auto Ja = dados.num_horarios() - 2;
  const auto Jb = dados.num_dias() - 2;

  // Ranges
  const auto rC = range(C);
  const auto rP = range(P);
  const auto rD = range(D);
  const auto rI = range(I);
  const auto rJ = range(J);
  const auto rJa = range(Ja);
  const auto rJb = range(Jb);

  // Dados
  const auto& pi = dados.pesos_constraints();
  const auto& h = dados.habilitado();
  const auto& H = dados.pertence_periodo();
  const auto& A = dados.disponivel();
  const auto& G = dados.dificil();
  const auto& F = dados.preferencia();
  const auto& Q = dados.aulas_desejadas();
  const auto& N = dados.horas_contrato();
  const auto& K = dados.carga_horaria();
  const auto& O = dados.oferecida();
  const auto& B = dados.aulas_geminadas();

  // Ambiente
  auto env = Solver::make_env();
  auto model = Solver::make_model(env);

  // Atalho para criação das variáveis de decisão
  const auto bin_var = [&model]() { return Solver::add_bin_var(model); };
  const auto int_var = [&model]() { return Solver::add_int_var(model); };

  //    Variáveis de decisão
  // D agendada no horário (I, J) para o professor P
  auto x = vec(P, D, I, J, bin_var);
  // Se a C tem aula em (I, J)
  auto r = vec(C, I, J, int_var);
  // Se existe uma janela de tamanho Ja começando em (I, J) para C
  auto alfa1 = vec(Ja, C, I, J, bin_var);
  // Número de janelas para turma C
  auto alfa = vec(C, int_var);
  // Se o professor P dá aula em J
  auto w = vec(P, J, bin_var);
  // Se existe uma janela de tamanho Jb começando em J para P
  auto beta1 = vec(Jb, P, J, bin_var);
  // Número de intervalos de trabalho para o professor P
  auto beta = vec(P, int_var);
  // Se a turma C possui aula em J
  auto g = vec(C, J, bin_var);
  // Número de dias de aula em C
  auto gama = vec(C, int_var);
  // Número de aulas que C tem aos sábados
  auto delta = vec(C, int_var);
  // Número de aulas seguidas de D em J
  auto epsilon = vec(D, J, int_var);
  // Número de aulas difíceis em J para C
  auto teta = vec(J, C, int_var);
  // Se existe aula difícil no último horário de C em J
  auto capa = vec(C, J, bin_var);
  // Número de disciplinas atribuídas a P que não são de sua preferência
  auto lambda = vec(P, int_var);
  // Número de aulas que excedem a preferência de P
  auto mi = vec(P, int_var);
  // Professor P lecionará a disciplina D
  auto Lec = vec(P, D, bin_var);
  // Indica se uma aula geminada começa em (I, J).
  auto gem = vec(P, D, I, J, bin_var);

  //    Objetivo
  // Termos
  auto janelas = sum<Solver>(alfa);
  auto intervalos = sum<Solver>(beta);
  auto compacto = sum<Solver>(gama);
  auto sabados = sum<Solver>(delta);
  auto seguidas = sum2D<Solver>(epsilon);
  auto dificeis_seguidas = sum2D<Solver>(teta);
  auto dificeis_ultimo = sum2D<Solver>(capa);
  auto pref_disc = sum<Solver>(lambda);
  auto pref_aula = sum<Solver>(mi);

  // Total
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

  Solver::set_min_objective(model, objetivo);

  //    Restrições
  // Disponibilidade do professor
  for (auto i : rI) 
    for (auto j : rJ) 
      for (auto p : rP) {
        auto soma = Solver::Expr_t{ 0 };
        for (auto d : rD) {
          soma += x[p][d][i][j];
        }
        Solver::add_constraint(model, soma <= A[p][i][j]);
      }

  // Conflito de aulas em um período
  for (auto i : rI) 
    for (auto j : rJ) 
      for (auto c : rC) {
        auto soma = Solver::Expr_t{ 0 };
        for (auto p : rP) 
          for (auto d : rD) 
            soma += x[p][d][i][j] * H[d][c];
        Solver::add_constraint(model, soma <= 1);
      }

  // Capacitação do professor
  for (auto p : rP) 
    for (auto d : rD) 
      for (auto i : rI) 
        for (auto j : rJ) 
          Solver::add_constraint(model, x[p][d][i][j] <= h[p][d]);

  // Impedem que uma disciplina tenha mais de um professor associado
  for (auto p : rP) 
    for (auto d : rD) 
      for (auto i : rI)  
        for (auto j : rJ) 
          Solver::add_constraint(model, Lec[p][d] >= x[p][d][i][j]);

  for (auto p : rP) 
    for (auto d : rD) {
      auto soma = Solver::Expr_t{ 0 };
      for (auto i : rI) 
        for (auto j : rJ) 
          soma += x[p][d][i][j];
      Solver::add_constraint(model, Lec[p][d] <= soma);
    }

  for (auto d : rD) {
    auto soma = Solver::Expr_t{ 0 };
    for (auto p : rP) 
      soma += Lec[p][d];
    Solver::add_constraint(model, soma == O[d]);
  }

  // Contrato do professor
  for (auto p : rP) { 
    auto soma = Solver::Expr_t{ 0 };
    for (auto d : rD) 
      for (auto i : rI) 
        for (auto j : rJ) 
          soma += x[p][d][i][j];
    Solver::add_constraint(model, soma <= N[p]);
  }

  // Carga horária das disciplinas
  for (auto d : rD) {
    auto soma = Solver::Expr_t{ 0 };
    for (auto p : rP) 
      for (auto i : rI) 
        for (auto j : rJ) 
          soma += x[p][d][i][j];
    Solver::add_constraint(model, soma == (K[d] * O[d]));
  }

  return {};
}

Json::Value
inline modelo(const DadosModelo& dados)
{
#ifdef GUROBI_ENABLED
  return solve<Gurobi>(dados);
#elif CPLEX_ENABLED
  return solve<Cplex>(dados);
#else
  throw std::runtime_error{ "Operação não suportada: instale o Gurobi ou CPLEX" };
#endif
}
