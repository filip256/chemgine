#pragma once

#include <unordered_map>

class Utils 
{
public:

	template<class KeyT1, class KeyT2, class Obj>
	static std::unordered_map<KeyT1, Obj> compose(
		const std::unordered_map<KeyT1, KeyT2>& map1,
		const std::unordered_map<KeyT2, Obj>& map2);
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