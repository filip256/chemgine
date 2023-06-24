#include "DataTable.hpp"
#include "AtomData.hpp"

template<class KeyT1, class KeyT2, class ObjT>
DataTable<KeyT1, KeyT2, ObjT>::DataTable() : table() 
{}

template<class KeyT1, class KeyT2, class ObjT>
const MultiIndexMap<KeyT1, KeyT2, ObjT>& DataTable<KeyT1, KeyT2, ObjT>::getData() const
{
	return table; 
}

template<class KeyT1, class KeyT2, class ObjT>
bool DataTable<KeyT1, KeyT2, ObjT>::contains(const KeyT1 key) const
{ 
	return table.containsKey1(key);
}

template<class KeyT1, class KeyT2, class ObjT>
bool DataTable<KeyT1, KeyT2, ObjT>::contains(const KeyT2 key) const
{
	return table.containsKey2(key);
}

template<class KeyT1, class KeyT2, class ObjT>
const ObjT& DataTable<KeyT1, KeyT2, ObjT>::operator[](const KeyT1 key) const
{
	return table.atKey1(key);
}

template<class KeyT1, class KeyT2, class ObjT>
const ObjT& DataTable<KeyT1, KeyT2, ObjT>::operator[](const KeyT2 key) const
{
	return table.atKey2(key);
}

template<class KeyT1, class KeyT2, class ObjT>
const ObjT* DataTable<KeyT1, KeyT2, ObjT>::findFirst(bool (*predicate) (const ObjT&)) const
{
	return table.findFirst(predicate);
}

template<class KeyT1, class KeyT2, class ObjT>
void DataTable<KeyT1, KeyT2, ObjT>::clear()
{
	table.clear();
}

template class DataTable<AtomIdType, std::string, AtomData>;