#ifndef UTIL_H
#define	UTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

namespace Util
{
	int getPosition(int y, int x, int z, int Y, int Z);
	void get3DMatrix(int pLinear, int* triDimensional, int X, int Y, int Z);

	std::vector<std::string>& strSplit(const std::string& s, char delim, std::vector<std::string>& elems);
	std::vector<std::string> strSplit(const std::string& s, char delim);

	double timeDiff(clock_t tf, clock_t t0);

	long long chronoDiff(std::chrono::time_point<std::chrono::high_resolution_clock> t1,
						 std::chrono::time_point<std::chrono::high_resolution_clock> t2);

	std::chrono::time_point<std::chrono::high_resolution_clock> now();

	int randomBetween(int min, int max);

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

};

#endif /* UTIL_H */

