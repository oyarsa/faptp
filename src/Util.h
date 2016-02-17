#ifndef UTIL_H
#define	UTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

class Util
{
public:
	Util();
	virtual ~Util();

	int getPosition(int y, int x, int z, int Y, int Z);
	void get3DMatrix(int pLinear, int* triDimensional, int X, int Y, int Z);

	std::vector<std::string>& strSplit(const std::string& s, char delim, std::vector<std::string>& elems);
	std::vector<std::string> strSplit(const std::string& s, char delim);

	double timeDiff(clock_t tf, clock_t t0);

	int randomBetween(int min, int max);

	template <typename Container, typename T, typename Compare = std::less<T>>
	static void insert_sorted(Container& c, const T& item, Compare cmp = Compare())
	{
		c.insert(
			std::lower_bound(begin(c), end(c), item, cmp),
			item
		);
	}

	template <typename Container, typename ForwardIterator,
	          typename Compare = std::less<typename ForwardIterator::value_type>>
	static void insert_sorted(Container& c, ForwardIterator first, ForwardIterator last, Compare cmp = Compare())
	{
		for (auto it = first; it != last; ++it) {
			c.insert(
				std::lower_bound(begin(c), end(c), *it, cmp),
				*it
			);
		}
	}
};

#endif /* UTIL_H */

