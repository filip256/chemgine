#pragma once

#include "MultiIndexMap.hpp"

template<class KeyT1, class KeyT2, class ObjT>
class DataTable
{
protected:
	MultiIndexMap<KeyT1, KeyT2, ObjT> table;

	template<class T = KeyT1, typename = std::enable_if_t<std::is_integral<T>::value>>
	T getFreeId() const;

public:
	DataTable() = default;
	DataTable(const MultiIndexMap<KeyT1, KeyT2, ObjT>&) = delete;

	const MultiIndexMap<KeyT1, KeyT2, ObjT>& getData() const;
	bool contains(const KeyT1 key) const;
	bool contains(const KeyT2 key) const;
	const ObjT& at(const KeyT1 key) const;
	const ObjT& at(const KeyT2 key) const;
	const std::vector<ObjT>& data() const;

	const size_t size() const;
	const ObjT& operator[](const size_t idx) const;
	ObjT& operator[](const size_t idx);

	virtual const ObjT* findFirst(bool (*predicate) (const ObjT&)) const;

	void clear();

	static constexpr size_t npos = static_cast<size_t>(-1);
};



template<class KeyT1, class KeyT2, class ObjT>
template<class T, typename>
T DataTable<KeyT1, KeyT2, ObjT>::getFreeId() const
{
	size_t id = 201;
	while (table.containsKey1(id) && id != 0) ++id; // overflow protection
	return id;
}
