#pragma once

#include <vector>
#include <optional>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <unordered_set>

namespace utils 
{
	template<typename KeyT1, typename KeyT2, typename ObjT>
	std::unordered_map<KeyT1, ObjT> compose(
		const std::unordered_map<KeyT1, KeyT2>& map1,
		const std::unordered_map<KeyT2, ObjT>& map2);

	template<typename T1, typename T2>
	std::pair<T2, T1> reversePair(const std::pair<T1, T2>& pair);

	template<typename KeyT, typename ObjT>
	std::unordered_map<ObjT, KeyT> reverseMap(const std::unordered_map<KeyT, ObjT>& map);

	template<typename KeyT, typename ObjT>
	std::vector<ObjT> extractValues(const std::unordered_map<KeyT, ObjT>& map);

	template<typename KeyT, typename ObjT>
	std::unordered_set<ObjT> extractUniqueValues(const std::unordered_map<KeyT, ObjT>& map);

	template<typename KeyT, typename ObjT>
	std::optional<ObjT> find(const std::unordered_map<KeyT, ObjT>& map, const KeyT& key);

	template<typename T1, typename T2>
	std::vector<T1> extractFirst(const std::vector<std::pair<T1, T2>>& vector);

	template<typename T1, typename T2>
	std::vector<T2> extractSecond(const std::vector<std::pair<T1, T2>>& vector);

	template<typename T>
	void swapAndPop(std::vector<T>& vector, typename std::vector<T>::iterator it);
	template<typename T>
	void swapAndPop(std::vector<T>& vector, const size_t idx);

	template<typename ObjT, typename CntT = size_t>
	std::vector<ObjT> flatten(
		const std::vector<std::pair<ObjT, CntT>>& vector);

	template<typename ObjT, typename CntT = size_t>
	std::unordered_map<ObjT, CntT> aggregate(const std::vector<ObjT>& vector);

	template<typename OutT, typename InT>
	std::vector<OutT> transform(const std::vector<InT>& vector);
	template<typename OutT, typename InT>
	std::vector<OutT> transform(std::vector<InT>&& vector);
	template<typename InT, typename CallableT>
	auto transform(const std::vector<InT>& vector, CallableT&& converter);

	template<typename InT1, typename InT2, typename OutT = std::pair<InT1, InT2>>
	std::vector<OutT> zip(std::vector<InT1>&& vector, const InT2& object);
	template<typename OutT, typename ObjT1, typename ObjT2>
	std::vector<OutT> zip(std::vector<ObjT1>&& vector, const ObjT2& object);

	template<typename T>
	std::vector<std::vector<T>> getArrangementsWithRepetitions(const std::vector<T>& vector, const size_t maxLength);

	template<typename T>
	std::vector<size_t> getSortingPermutation(const std::vector<T>& vector, bool (*lessThan)(const T&, const T&));
	template<typename T>
	void permute(std::vector<T>& vector, std::vector<size_t>& permutation);

	template<typename T, typename OutT = T>
	OutT getAveragedMedian(std::vector<T>& values);

	template<typename T>
	T copy(const T& obj);

	template<typename T>
	T min(T arg);
	template<typename T, typename... Args>
	T min(T arg1, Args... args);

	template<typename T>
	T max(T arg);
	template<typename T, typename... Args>
	T max(T arg1, Args... args);

	template<typename T>
	T closest(T z, T arg);
	template<typename T, typename... Args>
	T closest(T z, T arg1, Args... args);

	//
	// NPos
	//

	template <typename T>
	class NPos
	{
		static_assert(std::is_fundamental_v<T>, "NPos: T must be a primitive type.");

	public:
		static constexpr T value = std::conditional_t<
			std::is_unsigned_v<T>, std::integral_constant<T, static_cast<T>(-1)>,
			std::integral_constant<T, std::numeric_limits<T>::lowest()>
		>::value;
		static constexpr bool isNPos(const T& x);
	};

	template <typename T1, typename T2>
	class NPos<std::pair<T1, T2>>
	{
	public:
		using ObjT = std::pair<T1, T2>;

		static constexpr ObjT value = ObjT(NPos<T1>::value, NPos<T2>::value);
		static constexpr bool isNPos(const ObjT& pair);
	};

	template <typename... Ts>
	class NPos<std::tuple<Ts...>>
	{
	public:
		using ObjT = std::tuple<Ts...>;

		static constexpr ObjT value = ObjT(NPos<Ts>::value...);
		static constexpr bool isNPos(const ObjT& tuple);
	};

	template <typename T>
	constexpr bool NPos<T>::isNPos(const T& x)
	{
		return x == value;
	}

	template <typename T1, typename T2>
	constexpr bool NPos<std::pair<T1, T2>>::isNPos(const ObjT& pair)
	{
		return pair.first == NPos<T1>::value;
	}

	template <typename... Ts>
	constexpr bool NPos<std::tuple<Ts...>>::isNPos(const ObjT& tuple)
	{
		return std::get<0>(tuple) == NPos<std::tuple_element_t<0, ObjT>>::value;
	}

	template <typename T>
	constexpr T npos = NPos<T>::value;
	template <typename T>
	constexpr bool isNPos(const T& x);

	//
	// DerefIterator
	//

	template<typename IteratorT>
	class DerefIterator : public IteratorT
	{
	public:
		using pointer = std::iterator_traits<IteratorT>::value_type;
		using value_type = std::remove_pointer_t<pointer>;
		using reference = value_type&;
		using difference_type = std::iterator_traits<IteratorT>::difference_type;
		using iterator_category = std::iterator_traits<IteratorT>::iterator_category;
		using iterator_concept = IteratorT::iterator_concept;

		DerefIterator() = default;
		DerefIterator(IteratorT it) noexcept;

		reference operator*() const;
		pointer operator->() const;

		DerefIterator& operator++();
		DerefIterator operator++(int);
	};
};


template<typename T1, typename T2>
std::pair<T2, T1> utils::reversePair(
	const std::pair<T1, T2>& pair)
{
	return std::make_pair(pair.second, pair.first);
}

template<typename KeyT1, typename KeyT2, typename ObjT>
std::unordered_map<KeyT1, ObjT> utils::compose(
	const std::unordered_map<KeyT1, KeyT2>& map1,
	const std::unordered_map<KeyT2, ObjT>& map2)
{
	std::unordered_map<KeyT1, ObjT> result;
	result.reserve(map1.bucket_count());

	for (auto const& p : map1)
	{
		if (map2.contains(p.first))
			result.emplace(std::make_pair(p.first, map2.at(p.second)));
	}
	return result;
}

template<typename KeyT, typename ObjT>
std::unordered_map<ObjT, KeyT> utils::reverseMap(
	const std::unordered_map<KeyT, ObjT>& map)
{
	std::unordered_map<ObjT, KeyT> result;
	result.reserve(map.bucket_count());

	for (auto const& p : map)
		result.emplace(std::make_pair(p.second, p.first));

	return result;
}

template<typename KeyT, typename ObjT>
std::vector<ObjT> utils::extractValues(
	const std::unordered_map<KeyT, ObjT>& map)
{
	std::vector<ObjT> result;
	result.reserve(map.bucket_count());

	for (auto const& p : map)
		result.emplace_back(p.second);

	return result;
}

template<typename KeyT, typename ObjT>
std::unordered_set<ObjT> utils::extractUniqueValues(
	const std::unordered_map<KeyT, ObjT>& map)
{
	std::unordered_set<ObjT> result;
	result.reserve(map.bucket_count());

	for (auto const& p : map)
		result.emplace(p.second);

	return result;
}

template<typename KeyT, typename ObjT>
std::optional<ObjT> utils::find(const std::unordered_map<KeyT, ObjT>& map, const KeyT& key)
{
	if (const auto it = map.find(key); it != map.end())
		return it->second;
	return std::nullopt;
}

template<typename T1, typename T2>
std::vector<T1> utils::extractFirst(
	const std::vector<std::pair<T1, T2>>& vector)
{
	std::vector<T1> result;
	result.reserve(vector.size());

	const auto s = vector.size();
	for (size_t i = 0; i < s; ++i)
		result.emplace_back(vector[i].first);

	return result;
}

template<typename T1, typename T2>
std::vector<T2> utils::extractSecond(
	const std::vector<std::pair<T1, T2>>& vector)
{
	std::vector<T2> result;
	result.reserve(vector.size());

	const auto s = vector.size();
	for (size_t i = 0; i < s; ++i)
		result.emplace_back(vector[i].second);

	return result;
}

template<typename T>
void utils::swapAndPop(std::vector<T>& vector, typename std::vector<T>::iterator it)
{
	if (vector.size() <= 2)
	{
		vector.erase(it);
		return;
	}

	if (it == vector.end() - 1)
	{
		vector.pop_back();
		return;
	}

	auto temp = std::move(*it);
	*it = std::move(vector.back());
	vector.back() = std::move(temp);

	vector.pop_back();
}

template<typename T>
void utils::swapAndPop(std::vector<T>& vector, const size_t idx)
{
	swapAndPop(vector, vector.begin() + idx);
}

template<typename ObjT, typename CntT>
std::vector<ObjT> utils::flatten(const std::vector<std::pair<ObjT, CntT>>& vector)
{
	std::vector<ObjT> result;
	result.reserve(vector.size());

	for (size_t i = 0; i < vector.size(); ++i)
		for (CntT j = 0; j < vector[i].second; ++j)
			result.emplace_back(vector[i].first);

	return result;
}

template<typename ObjT, typename CntT>
std::unordered_map<ObjT, CntT> utils::aggregate(const std::vector<ObjT>& vector)
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

template<typename OutT, typename InT>
std::vector<OutT> utils::transform(const std::vector<InT>& vector)
{
	std::vector<OutT> result;
	result.reserve(vector.size());

	std::transform(vector.begin(), vector.end(), std::back_inserter(result),
		[](const InT& item) { return OutT(item); });

	return result;
}

template<typename OutT, typename InT>
std::vector<OutT> utils::transform(std::vector<InT>&& vector)
{
	std::vector<OutT> result;
	result.reserve(vector.size());

	std::transform(std::make_move_iterator(vector.begin()), std::make_move_iterator(vector.end()), std::back_inserter(result),
		[](InT&& item) { return OutT(std::move(item)); });

	return result;
}

template<typename InT, typename CallableT>
auto utils::transform(const std::vector<InT>& vector, CallableT&& converter)
{
	using OutT = std::decay_t<decltype(converter(std::declval<InT>()))>;
	std::vector<OutT> result;
	result.reserve(vector.size());

	std::transform(vector.begin(), vector.end(), std::back_inserter(result),
		[cvt = std::forward<CallableT>(converter)](const InT& item) { return cvt(item); });

	return result;
}

template<typename InT1, typename InT2, typename OutT>
std::vector<OutT> utils::zip(std::vector<InT1>&& vector, const InT2& object)
{
	std::vector<OutT> result;
	result.reserve(vector.size());

	std::transform(std::make_move_iterator(vector.begin()), std::make_move_iterator(vector.end()), std::back_inserter(result),
		[&object](InT1&& item)
		{
			if constexpr (std::is_same_v<std::pair<InT1, InT2>, OutT>)
				return std::make_pair(std::move(item), object);
			else
				return OutT(std::move(item), object);
		});

	return result;
}

template<typename OutT, typename InT1, typename InT2>
std::vector<OutT> utils::zip(std::vector<InT1>&& vector, const InT2& object)
{
	return zip<InT1, InT2, OutT>(std::move(vector), object);
}

template<typename T>
static void getArrangementsWithRepetitions(const std::vector<T>& vector, const size_t maxLength,
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

template<typename T>
std::vector<std::vector<T>> utils::getArrangementsWithRepetitions(
	const std::vector<T>& vector, const size_t maxLength)
{
	// TODO: implement iteratively
	std::vector<std::vector<T>> result;
	result.reserve(static_cast<size_t>(std::pow(vector.size(), maxLength)) + 1);
	std::vector<T> current;
	current.reserve(maxLength);

	::getArrangementsWithRepetitions(vector, maxLength, current, result);
	return result;
}

template<typename T>
std::vector<size_t> utils::getSortingPermutation(const std::vector<T>& vector, bool (*lessThan)(const T&, const T&))
{
	// Initialize indexMap[i] = i
	std::vector<size_t> indexMap;
	indexMap.reserve(vector.size());
	for (size_t i = 0; i < vector.size(); ++i)
		indexMap.emplace_back(i);

	// Generate a mapping between each index in the sorted vector and the index of the same element in the original vector.
	std::sort(indexMap.begin(), indexMap.end(), [&vector, lessThan](const auto& lhs, const auto& rhs)
		{
			return lessThan(vector[lhs], vector[rhs]);
		});

	// Reverse the mapping to obtain the permutation
	std::vector<size_t> permutation(vector.size());
	for (size_t i = 0; i < vector.size(); ++i)
		permutation[indexMap[i]] = i;

	return permutation;
}

template<typename T>
void utils::permute(std::vector<T>& vector, std::vector<size_t>& permutation)
{
	static constexpr auto npos = static_cast<size_t>(-1);

	for (size_t i = 0; i < vector.size(); ++i)
	{
		if (permutation[i] == npos || permutation[i] == i)
			continue;

		auto current = i;
		auto currentObj = std::move(vector[current]);

		// Follow the permutation cycle
		do
		{
			std::swap(vector[permutation[current]], currentObj);

			const auto next = permutation[current];
			permutation[current] = npos;
			current = next;

		} while (permutation[current] != npos);
	}
}

template<typename T, typename OutT>
OutT utils::getAveragedMedian(std::vector<T>& values)
{
	const auto mid = values.size() / 2;
	std::nth_element(values.begin(), values.begin() + mid, values.end());

	if (values.size() % 2 == 1)
		return values[mid];

	// For even sizes we need to find the (mid-1)th element too.
	const auto mid1 = values[mid];
	std::nth_element(values.begin(), values.begin() + mid - 1, values.end());
	const auto mid2 = values[mid - 1];

	return round_cast<OutT>((mid1 + mid2) / 2.0f);
}

template<typename T>
T utils::copy(const T& obj)
{
	return obj;
}

template<typename T>
T utils::min(T arg) {
	return arg;
}

template<typename T, typename... Args>
T utils::min(T arg1, Args... args) {
	return std::min(arg1, max(args...));
}

template<typename T>
T utils::max(T arg) {
	return arg;
}

template<typename T, typename... Args>
T utils::max(T arg1, Args... args) {
	return std::max(arg1, max(args...));
}

template<typename T>
T utils::closest(T z, T arg)
{
	return arg;
}

template<typename T, typename... Args>
T utils::closest(T z, T arg1, Args... args)
{
	const auto rest = closest(z, args...);
	return (std::abs(z - arg1) < std::abs(z - rest)) ? arg1 : rest;
}

//
// NPos
//

template <typename T>
constexpr bool utils::isNPos(const T& x)
{
	return NPos<T>::isNPos(x);
}

//
// DerefIterator
//

template<typename IteratorT>
utils::DerefIterator<IteratorT>::DerefIterator(IteratorT it) noexcept :
	IteratorT(it)
{}

template<typename IteratorT>
utils::DerefIterator<IteratorT>::reference utils::DerefIterator<IteratorT>::operator*() const
{
	return **static_cast<const IteratorT&>(*this);
}

template<typename IteratorT>
utils::DerefIterator<IteratorT>::pointer utils::DerefIterator<IteratorT>::operator->() const
{
	return *static_cast<const IteratorT&>(*this);
}

template<typename IteratorT>
utils::DerefIterator<IteratorT>& utils::DerefIterator<IteratorT>::operator++()
{
	IteratorT::operator++();
	return *this;
}

template<typename IteratorT>
utils::DerefIterator<IteratorT> utils::DerefIterator<IteratorT>::operator++(int)
{
	DerefIterator tmp = *this;
	++(*this);
	return tmp;
}