#pragma once

#include<unordered_set>
#include<set>
#include<unordered_map>
#include<map>

/// <summary>
/// Interface for containers with a contains method, useful for avoiding conversions.
/// The interface has the same lifetime as the underlying container.
/// In order to afford maps with different key and value types, the function receiving a ContainsInterface 
/// can generalize the value type argument. In this case ContainsInterface ctors must be called explicitly 
/// to allow type deduction in the receiving function.
/// </summary>
template<class KeyT, class ObjT = KeyT>
class ContainsInterface
{
private:
	const uint8_t type;
	union Container
	{
		const std::set<KeyT>* set;
		const std::unordered_set<KeyT>* uset;
		const std::map<KeyT, ObjT>* map;
		const std::unordered_map<KeyT, ObjT>* umap;

		Container(const std::set<KeyT>& set) noexcept : set(&set) {};
		Container(const std::unordered_set<KeyT>& uset) noexcept : uset(&uset) {};
		Container(const std::map<KeyT, ObjT>& map) noexcept : map(&map) {};
		Container(const std::unordered_map<KeyT, ObjT>& umap) noexcept : umap(&umap) {};

	} container;

public:
	ContainsInterface(const std::set<KeyT>& set) noexcept;
	ContainsInterface(const std::unordered_set<KeyT>& uset) noexcept;
	ContainsInterface(const std::map<KeyT, ObjT>& map) noexcept;
	ContainsInterface(const std::unordered_map<KeyT, ObjT>& umap) noexcept;

	bool contains(const KeyT& item) const;
};

template<class KeyT, class ObjT>
ContainsInterface<KeyT, ObjT>::ContainsInterface(const std::set<KeyT>& set) noexcept :
	type(1),
	container(set)
{}

template<class KeyT, class ObjT>
ContainsInterface<KeyT, ObjT>::ContainsInterface(const std::unordered_set<KeyT>& uset) noexcept :
	type(2),
	container(uset)
{}

template<class KeyT, class ObjT>
ContainsInterface<KeyT, ObjT>::ContainsInterface(const std::map<KeyT, ObjT>& map) noexcept:
	type(3),
	container(map)
{}

template<class KeyT, class ObjT>
ContainsInterface<KeyT, ObjT>::ContainsInterface(const std::unordered_map<KeyT, ObjT>& umap) noexcept :
	type(4),
	container(umap)
{}


template<class KeyT, class ObjT>
bool ContainsInterface<KeyT, ObjT>::contains(const KeyT& item) const
{
	switch (type)
	{
	case 1:
		return container.set->contains(item);
	case 2:
		return container.uset->contains(item);
	case 3:
		return container.map->contains(item);
	case 4:
		return container.umap->contains(item);
	}
}