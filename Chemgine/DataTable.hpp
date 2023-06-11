#pragma once

#include <unordered_map>

template<typename KeyT, class ObjT>
class DataTable
{
protected:
	std::unordered_map<KeyT, ObjT> table;

public:
	DataTable();
	DataTable(const DataTable<KeyT, ObjT>&) = delete;

	const std::unordered_map<KeyT, ObjT>& getData() const;
	bool contains(const KeyT id) const;
	const ObjT& operator[](const KeyT id) const;
};