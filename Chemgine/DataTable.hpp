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

	/// <summary>
	/// Returns a pointer to the first element that satisfies the given predicate, or nullptr if no such element exists.
	/// Complexity: O(size)
	/// </summary>
	virtual const ObjT* first(bool (*predicate) (const ObjT&)) const;
};