#ifndef UTIL_H
#define    UTIL_H

#include <string_view>
#include <vector>
#include <algorithm>
#include <iostream>
#include <chrono>

namespace Util
{
constexpr int getPosition(int y, int x, int z, int Y, int Z);

// Bloco, dia, camada
void get3DMatrix(std::size_t pLinear, int triDimensional[3], int X, int Y, int Z);

std::vector<std::string>& strSplit(const std::string& s, char delim, std::vector<std::string>& elems);
std::vector<std::string> strSplit(const std::string& s, char delim);

constexpr double timeDiff(clock_t tf, clock_t t0)
{
  return (tf - t0) / 1000000.0 * 1000;
}

// Calcula a distância entre t_end - t_begin em milissegundos
long long chronoDiff(
    std::chrono::time_point<std::chrono::high_resolution_clock> t_end,
    std::chrono::time_point<std::chrono::high_resolution_clock> t_begin);

std::chrono::time_point<std::chrono::high_resolution_clock> now();

int randomBetween(int min, int max);
double randomDouble();

constexpr int warpIntervalo(int i, int tamIntervalo, int comecoIntervalo)
{
    return (i - comecoIntervalo) % tamIntervalo + comecoIntervalo;
}

// Insere um item num container ordenado
template <typename Container, typename T, typename Compare = std::less<T>>
void insert_sorted(Container& c, const T& item, Compare cmp = Compare())
{
    c.insert(
        std::upper_bound(begin(c), end(c), item, cmp),
        item
    );
}

// Insere todos os item de `first` a `last` num container ordenado
template <typename Container, typename ForwardIterator,
          typename Compare = std::less<typename ForwardIterator::value_type>>
void insert_sorted(Container& c, ForwardIterator first,
                   ForwardIterator last, Compare cmp = Compare())
{
    for (; first != last; ++first) {
        insert_sorted(c, *first, cmp);
    }
}

// Une uma lista de nomes de pastas e um arquivo em uma string de forma portável
std::string join_path(const std::vector<std::string>& folders,
                      const std::string& file = "");

// Cria uma pasta de forma portável
void create_folder(const std::string& path);

std::size_t hash_string(const std::string& str);

constexpr int
factorial(int n)
{
  auto x = 1;
  for (auto i = 2; i <= n; i++) {
    x *= i;
  }
  return x;
}

constexpr int
fast_ceil(double x)
{
  const auto ix = static_cast<int>(x);
  return ix + (x > ix);
}

constexpr int
fast_floor(double x)
{
  const auto ix = static_cast<int>(x);
  return ix - (x < ix);
}

template <typename Container>
auto
randomChoice(Container& c)
{
  const auto n = randomBetween(0, static_cast<int>(c.size()));
  auto it = begin(c);
  std::advance(it, n);
  return it;
}

template <typename T>
std::array<T, 100>
gen_chance_array(const std::vector<std::pair<T, int>>& chances)
{
  std::array<T, 100> chance_array;
  auto it = begin(chance_array);

  for (const auto& p : chances) {
    it = std::fill_n(it, p.second, p.first);
  }

  if (it != end(chance_array)) {
    std::fill(it, end(chance_array), chances.back().first);
  }

  return chance_array;
}

template <class T>
void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename S, typename T>
struct hash_pair
{
    size_t operator()(const std::pair<S, T>& p) const
    {
        size_t seed{0};
        hash_combine(seed, p.first);
        hash_combine(seed, p.second);
        return seed;
    }
};

std::string date_time();

void logprint(std::ostream& log, std::string_view data);

std::string get_computer_name();

} // namespace Util

#endif /* UTIL_H */

