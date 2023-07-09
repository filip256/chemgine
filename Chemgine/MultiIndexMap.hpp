#pragma once

#include <vector>
#include <unordered_map>

template <class KeyT1, class KeyT2, class ObjT>
class MultiIndexMap
{
private:
	std::vector<ObjT> objects;
	std::unordered_map<KeyT1, size_t> indexer1;
	std::unordered_map<KeyT2, size_t> indexer2;

public:
	MultiIndexMap();

	bool emplace(const KeyT1& key1, const KeyT2& key2, ObjT&& object);

	/// <summary>
	/// Retrieves the element with the given key.
	/// Complexity: O(1)
	/// </summary>
	const ObjT& atKey1(const KeyT1& key1) const;

	/// <summary>
	/// Retrieves the element with the given key.
	/// Complexity: O(1)
	/// </summary>
	const ObjT& atKey2(const KeyT2& key2) const;


	bool containsKey1(const KeyT1& key1) const;
	bool containsKey2(const KeyT2& key2) const;

	/// <summary>
	/// Returns a pointer to the first element that satisfies the given predicate, or nullptr if no such element exists.
	/// Complexity: O(n)
	/// </summary>
	const ObjT* findFirst(bool (*predicate) (const ObjT&)) const;

	/// <summary>
	/// Used to iterate over elements with vector-like efficiency.
	/// Complexity: O(1)
	/// </summary>
	const ObjT& operator[](const size_t idx) const;

	/// <summary>
	/// Used to iterate over elements with vector-like efficiency.
	/// Complexity: O(1)
	/// </summary>
	ObjT& operator[](const size_t idx);

	/// <summary>
	/// Clears the contents of the container
	/// Complexity: O(n)
	/// </summary>
	void clear();

	/// <summary>
	/// Returns the number of elements in the container
	/// </summary>
	size_t size() const;

	/// <summary>
	/// Returns true if the container is empty
	/// </summary>
	bool empty() const;
};



template <class KeyT1, class KeyT2, class ObjT>
MultiIndexMap<KeyT1, KeyT2, ObjT>::MultiIndexMap()
{}

template <class KeyT1, class KeyT2, class ObjT>
bool MultiIndexMap<KeyT1, KeyT2, ObjT>::emplace(const KeyT1& key1, const KeyT2& key2, ObjT&& object)
{
	if (containsKey1(key1) || containsKey2(key2))
		return false;

	objects.emplace_back(std::move(object));
	indexer1.emplace(std::move(std::make_pair(key1, objects.size() - 1)));
	indexer2.emplace(std::move(std::make_pair(key2, objects.size() - 1)));
	return true;
}

template <class KeyT1, class KeyT2, class ObjT>
const ObjT& MultiIndexMap<KeyT1, KeyT2, ObjT>::atKey1(const KeyT1& key1) const
{
	return objects[indexer1.at(key1)];
}

template <class KeyT1, class KeyT2, class ObjT>
const ObjT& MultiIndexMap<KeyT1, KeyT2, ObjT>::atKey2(const KeyT2& key2) const
{
	return objects[indexer2.at(key2)];
}

template <class KeyT1, class KeyT2, class ObjT>
bool MultiIndexMap<KeyT1, KeyT2, ObjT>::containsKey1(const KeyT1& key1) const
{
	return indexer1.contains(key1);
}

template <class KeyT1, class KeyT2, class ObjT>
bool MultiIndexMap<KeyT1, KeyT2, ObjT>::containsKey2(const KeyT2& key2) const
{
	return indexer2.contains(key2);
}

template <class KeyT1, class KeyT2, class ObjT>
const ObjT& MultiIndexMap<KeyT1, KeyT2, ObjT>::operator[](const size_t idx) const
{
	return objects[idx];
}

template <class KeyT1, class KeyT2, class ObjT>
ObjT& MultiIndexMap<KeyT1, KeyT2, ObjT>::operator[](const size_t idx)
{
	return objects[idx];
}

template <class KeyT1, class KeyT2, class ObjT>
const ObjT* MultiIndexMap<KeyT1, KeyT2, ObjT>::findFirst(bool (*predicate) (const ObjT&)) const
{
	for (size_t i = 0; i < objects.size(); ++i)
		if (predicate(objects[i]))
			return &objects[i];
	return nullptr;
}

template <class KeyT1, class KeyT2, class ObjT>
void MultiIndexMap<KeyT1, KeyT2, ObjT>::clear()
{
	objects.clear();
	indexer1.clear();
	indexer2.clear();
}

template <class KeyT1, class KeyT2, class ObjT>
size_t MultiIndexMap<KeyT1, KeyT2, ObjT>::size() const
{
	return objects.size();
}

template <class KeyT1, class KeyT2, class ObjT>
bool MultiIndexMap<KeyT1, KeyT2, ObjT>::empty() const
{
	return objects.empty();
}