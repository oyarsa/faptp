#ifndef UTIL_H
#define	UTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

#include <gsl.h>

namespace Util
{
	int getPosition(int y, int x, int z, int Y, int Z);
	// Bloco, dia, camada
	void get3DMatrix(int pLinear, int triDimensional[3], int X, int Y, int Z);

	std::vector<std::string>& strSplit(const std::string& s, char delim, std::vector<std::string>& elems);
	std::vector<std::string> strSplit(const std::string& s, char delim);

	double timeDiff(clock_t tf, clock_t t0);

	// Calcula a distância entre t_end - t_begin em milissegundos
	long long chronoDiff(
		std::chrono::time_point<std::chrono::high_resolution_clock> t_end,
		std::chrono::time_point<std::chrono::high_resolution_clock> t_begin);

	std::chrono::time_point<std::chrono::high_resolution_clock> now();

	int randomBetween(int min, int max);
	double randomDouble();

	int warpIntervalo(int i, int tamIntervalo, int comecoIntervalo);

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
		for (auto it = first; it != last; ++it) {
			insert_sorted(c, *it, cmp);
		}
	}

	// Une uma lista de nomes de pastas e um arquivo em uma string de forma portável
	std::string join_path(const std::vector<std::string>& folders, 
								 const std::string& file = "");

	// Cria uma pasta de forma portável
	void create_folder(const std::string& path);

	std::size_t hash_string(const std::string& str);

	inline int fast_ceil(double x)
	{
		return int(x) + (x > int(x));
	}

	inline int fast_floor(double x)
	{
		return int(x) - (x < int(x));
	}

	template <typename Container>
	auto& randomChoice(Container& c)
	{
		auto n = randomBetween(0, c.size());
		auto it = begin(c);
		std::advance(it, n);
		return *it;
	}

	template <typename T>
	std::array<T, 100> gen_chance_array(const std::vector<std::pair<T, int>>& chances)
	{
		std::array<T, 100> chance_array{};
		auto it = begin(chance_array);

		for (const auto& p : chances) {
			it = std::fill_n(it, p.second, p.first);
		}

		if (it != end(chance_array)) {
			std::fill(it, end(chance_array), chances.back().first);
		}

		return chance_array;
	}
	
};

#endif /* UTIL_H */

