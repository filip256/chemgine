#include "DataTable.hpp"
#include "AtomData.hpp"

template <typename KeyT, class ObjT>
DataTable<KeyT, ObjT>::DataTable() : table() 
{}

template <typename KeyT, class ObjT>
const std::unordered_map<KeyT, ObjT>& DataTable<KeyT, ObjT>::getData() const 
{
	return table; 
}

template <typename KeyT, class ObjT>
bool DataTable<KeyT, ObjT>::contains(const KeyT id) const 
{ 
	return table.find(id) != table.end();
}

template <typename KeyT, class ObjT>
const ObjT& DataTable<KeyT, ObjT>::operator[](const KeyT id) const
{
	return table.at(id);
}

template class DataTable<AtomIdType, AtomData>;