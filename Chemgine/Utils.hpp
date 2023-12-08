#pragma once

#include <unordered_map>
#include <unordered_set>

class Utils 
{
public:

	template<class KeyT1, class KeyT2, class Obj>
	static std::unordered_map<KeyT1, Obj> compose(
		const std::unordered_map<KeyT1, KeyT2>& map1,
		const std::unordered_map<KeyT2, Obj>& map2);

	template<class Key, class Obj>
	static std::unordered_map<Obj, Key> reverseMap(
		const std::unordered_map<Key, Obj>& map);

	template<class Key, class Obj>
	static std::unordered_set<Obj> extractValues(
		const std::unordered_map<Key, Obj>& map);
};

template<class KeyT1, class KeyT2, class Obj>
std::unordered_map<KeyT1, Obj> Utils::compose(
	const std::unordered_map<KeyT1, KeyT2>& map1,
	const std::unordered_map<KeyT2, Obj>& map2)
{
	std::unordered_map<KeyT1, Obj> result;
	result.reserve(map1.bucket_count());

	for (auto const& p : map1)
	{
		if (map2.contains(p.first))
			result.insert(std::make_pair(p.first, map2.at(p.second)));
	}
	return result;
}

template<class Key, class Obj>
static std::unordered_map<Obj, Key> Utils::reverseMap(
	const std::unordered_map<Key, Obj>& map)
{
	std::unordered_map<Obj, Key> result;
	result.reserve(map.bucket_count());

	for (auto const& p : map)
	{
		result.insert(std::make_pair(p.second, p.first));
	}
	return result;
}

template<class Key, class Obj>
static std::unordered_set<Obj> Utils::extractValues(
	const std::unordered_map<Key, Obj>& map)
{
	std::unordered_set<Obj> result;
	result.reserve(map.bucket_count());

	for (auto const& p : map)
	{
		result.insert(p.second);
	}
	return result;
}