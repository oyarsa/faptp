#pragma once

#ifdef USE_TSL_HASHMAP
	#include <tsl/hopscotch_set.h>
	#include <tsl/robin_map.h>

	template<typename K, typename V, typename Hash = std::hash<K>>
	using hash_map = tsl::robin_map<K, V, Hash>;

	template<typename T, typename Hash = std::hash<T>>
	using hash_set = tsl::hopscotch_set<T, Hash>;
#else
	#include <unordered_map>
	#include <unordered_set>

	template<typename K, typename V, typename Hash = std::hash<K>>
	using hash_map = std::unordered_map<K, V, Hash>;

	template<typename T, typename Hash = std::hash<T>>
	using hash_set = std::unordered_set<T, Hash>;
#endif

