#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

class Utils 
{
private:
	template<class T>
	static void getArrangementsWithRepetitions(
		const std::vector<T>& vector, const size_t maxLength,
		std::vector<T>& current, std::vector<std::vector<T>>& result);

public:
	//template<class T>
	//static std::vector<T> toSortedSetVector(std::vector<T>&& vect);

	template<class KeyT1, class KeyT2, class Obj>
	static std::unordered_map<KeyT1, Obj> compose(
		const std::unordered_map<KeyT1, KeyT2>& map1,
		const std::unordered_map<KeyT2, Obj>& map2);

	template<class T1, class T2>
	static std::pair<T2, T1> reversePair(
		const std::pair<T1, T2>& pair);

	template<class Key, class Obj>
	static std::unordered_map<Obj, Key> reverseMap(
		const std::unordered_map<Key, Obj>& map);

	template<class Key, class Obj>
	static std::vector<Obj> extractValues(
		const std::unordered_map<Key, Obj>& map);

	template<class Key, class Obj>
	static std::unordered_set<Obj> extractUniqueValues(
		const std::unordered_map<Key, Obj>& map);

	template<class T1, class T2>
	static std::vector<T1> extractFirst(
		const std::vector<std::pair<T1, T2>>& vector);

	template<class T1, class T2>
	static std::vector<T2> extractSecond(
		const std::vector<std::pair<T1, T2>>& vector);

	template<class ObjT, class CntT = size_t>
	static std::vector<ObjT> flatten(
		const std::vector<std::pair<ObjT, CntT>>& vector);

	template<class ObjT, class CntT = size_t>
	static std::unordered_map<ObjT, CntT> aggregate(
		const std::vector<ObjT>& vector);

	template<class T>
	static std::vector<std::vector<T>> getArrangementsWithRepetitions(
		const std::vector<T>& vector, const size_t maxLength);

	template<class T>
	static T copy(const T& obj);
};


//template<class T>
//std::vector<T> Utils::toSortedSetVector(std::vector<T>&& vect)
//{
//	std::sort(vect.begin(), vect.end());
//	vect.erase(std::unique(vect.begin(), vect.end()), vect.end());
//	return std::move(vect);
//}

template<class T1, class T2>
static std::pair<T2, T1> Utils::reversePair(
	const std::pair<T1, T2>& pair)
{
	return std::make_pair(pair.second, pair.first);
}

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
			result.emplace(std::make_pair(p.first, map2.at(p.second)));
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
		result.emplace(std::make_pair(p.second, p.first));

	return result;
}

template<class Key, class Obj>
static std::vector<Obj> Utils::extractValues(
	const std::unordered_map<Key, Obj>& map)
{
	std::vector<Obj> result;
	result.reserve(map.bucket_count());

	for (auto const& p : map)
		result.emplace_back(p.second);

	return result;
}

template<class Key, class Obj>
static std::unordered_set<Obj> Utils::extractUniqueValues(
	const std::unordered_map<Key, Obj>& map)
{
	std::unordered_set<Obj> result;
	result.reserve(map.bucket_count());

	for (auto const& p : map)
		result.emplace(p.second);

	return result;
}

template<class T1, class T2>
static std::vector<T1> Utils::extractFirst(
	const std::vector<std::pair<T1, T2>>& vector)
{
	std::vector<T1> result;
	result.reserve(vector.size());

	const auto s = vector.size();
	for (size_t i = 0; i < s; ++i)
		result.emplace_back(vector[i].first);

	return result;
}

template<class T1, class T2>
static std::vector<T2> Utils::extractSecond(
	const std::vector<std::pair<T1, T2>>& vector)
{
	std::vector<T2> result;
	result.reserve(vector.size());

	const auto s = vector.size();
	for (size_t i = 0; i < s; ++i)
		result.emplace_back(vector[i].second);

	return result;
}

template<class ObjT, class CntT>
std::vector<ObjT> Utils::flatten(
	const std::vector<std::pair<ObjT, CntT>>& vector)
{
	std::vector<ObjT> result;
	result.reserve(vector.size());

	for (size_t i = 0; i < vector.size(); ++i)
		for (CntT j = 0; j < vector[i].second; ++j)
			result.emplace_back(vector[i].first);

	return result;
}

template<class ObjT, class CntT>
static std::unordered_map<ObjT, CntT> Utils::aggregate(
	const std::vector<ObjT>& vector)
{
	std::unordered_map<ObjT, CntT> result;
	for (size_t i = 0; i < vector.size(); ++i)
	{
		const auto temp = result.find(vector[i]);
		if (temp == result.end())
			result.emplace(std::make_pair(vector[i], 1));
		else
			++temp->second;
	}

	return result;
}

template<class T>
static void Utils::getArrangementsWithRepetitions(
	const std::vector<T>& vector, const size_t maxLength,
	std::vector<T>& current, std::vector<std::vector<T>>& result)
{
	if (current.size() <= maxLength)
		result.emplace_back(current);

	if (current.size() >= maxLength)
		return;

	for (size_t i = 0; i < vector.size(); ++i) 
	{
		current.emplace_back(vector[i]);
		getArrangementsWithRepetitions(vector, maxLength, current, result);
		current.pop_back();
	}
}

template<class T>
static std::vector<std::vector<T>> Utils::getArrangementsWithRepetitions(
	const std::vector<T>& vector, const size_t maxLength)
{
	// TODO: implement iteratively
	std::vector<std::vector<T>> result;
	result.reserve(std::powf(vector.size(), maxLength) + 1);
	std::vector<T> current;
	current.reserve(maxLength);

	getArrangementsWithRepetitions(vector, maxLength, current, result);
	return result;
}

template<class T>
static T Utils::copy(const T& obj)
{
	return obj;
}
