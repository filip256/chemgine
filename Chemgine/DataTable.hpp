#pragma once

#include "MultiIndexMap.hpp"

template<class KeyT1, class KeyT2, class ObjT>
class DataTable
{
protected:
	MultiIndexMap<KeyT1, KeyT2, ObjT> table;

public:
	DataTable();
	DataTable(const MultiIndexMap<KeyT1, KeyT2, ObjT>&) = delete;

	const MultiIndexMap<KeyT1, KeyT2, ObjT>& getData() const;
	bool contains(const KeyT1 key) const;
	bool contains(const KeyT2 key) const;
	const ObjT& operator[](const KeyT1 key) const;
	const ObjT& operator[](const KeyT2 key) const;

	virtual const ObjT* findFirst(bool (*predicate) (const ObjT&)) const;

	void clear();
};