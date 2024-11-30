#pragma once

#include <vector>
#include <unordered_map>

template <class KeyT1, class ObjT>
class IndexMap
{
protected:
	std::vector<ObjT> objects;
	std::unordered_map<KeyT1, size_t> indexer1;

public:
	IndexMap() = default;

	bool emplace(const KeyT1& key1, ObjT&& object);

	/// <summary>
	/// Retrieves the element with the given key.
	/// Complexity: O(1)
	/// </summary>
	const ObjT& atKey1(const KeyT1& key1) const;

	/// <summary>
	/// Returns a reference to the underlying data container.
	/// </summary>
	/// <returns></returns>
	const std::vector<ObjT>& data() const;

	bool containsKey1(const KeyT1& key1) const;

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



template <class KeyT1, class ObjT>
bool IndexMap<KeyT1, ObjT>::emplace(const KeyT1& key1, ObjT&& object)
{
	if (containsKey1(key1))
		return false;

	objects.emplace_back(std::move(object));
	indexer1.emplace(std::move(std::make_pair(key1, objects.size() - 1)));
	return true;
}

template <class KeyT1, class ObjT>
const ObjT& IndexMap<KeyT1, ObjT>::atKey1(const KeyT1& key1) const
{
	return objects[indexer1.at(key1)];
}

template <class KeyT1, class ObjT>
const std::vector<ObjT>& IndexMap<KeyT1, ObjT>::data() const
{
	return objects;
}

template <class KeyT1, class ObjT>
bool IndexMap<KeyT1, ObjT>::containsKey1(const KeyT1& key1) const
{
	return indexer1.contains(key1);
}

template <class KeyT1, class ObjT>
const ObjT& IndexMap<KeyT1, ObjT>::operator[](const size_t idx) const
{
	return objects[idx];
}

template <class KeyT1, class ObjT>
ObjT& IndexMap<KeyT1, ObjT>::operator[](const size_t idx)
{
	return objects[idx];
}

template <class KeyT1, class ObjT>
const ObjT* IndexMap<KeyT1, ObjT>::findFirst(bool (*predicate) (const ObjT&)) const
{
	for (size_t i = 0; i < objects.size(); ++i)
		if (predicate(objects[i]))
			return &objects[i];
	return nullptr;
}

template <class KeyT1, class ObjT>
void IndexMap<KeyT1, ObjT>::clear()
{
	objects.clear();
	indexer1.clear();
}

template <class KeyT1, class ObjT>
size_t IndexMap<KeyT1, ObjT>::size() const
{
	return objects.size();
}

template <class KeyT1, class ObjT>
bool IndexMap<KeyT1, ObjT>::empty() const
{
	return objects.empty();
}