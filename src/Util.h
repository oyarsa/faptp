#ifndef UTIL_H
#define	UTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>

class Util
{
public:
	int getPosition(int y, int x, int z, int Y, int Z);
	void get3DMatrix(int pLinear, int* triDimensional, int X, int Y, int Z);

	std::vector<std::string>& strSplit(const std::string& s, char delim, std::vector<std::string>& elems);
	std::vector<std::string> strSplit(const std::string& s, char delim);

	double timeDiff(clock_t tf, clock_t t0);

	static long long chronoDiff(std::chrono::time_point<std::chrono::system_clock> t1,
								std::chrono::time_point<std::chrono::system_clock> t2);

	int randomBetween(int min, int max);

	// Insere um item num container ordenado
	template <typename Container, typename T, typename Compare = std::less<T>>
	static void insert_sorted(Container& c, const T& item, Compare cmp = Compare())
	{
		c.insert(
			std::upper_bound(begin(c), end(c), item, cmp),
			item
		);
	}

	// Insere todos os item de `first` a `last` num container ordenado
	template <typename Container, typename ForwardIterator,
	          typename Compare = std::less<typename ForwardIterator::value_type>>
	static void insert_sorted(Container& c, ForwardIterator first, ForwardIterator last, Compare cmp = Compare())
	{
		for (auto it = first; it != last; ++it) {
			insert_sorted(c, *it, cmp);
		}
	}

	// Une uma lista de nomes de pastas e um arquivo em uma string de forma portável
	static std::string join_path(const std::vector<std::string>& folders, 
								 const std::string& file = "");

	// Cria uma pasta de forma portável
	static void create_folder(const std::string& path);

};

#endif /* UTIL_H */

