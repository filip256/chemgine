#include "DataTable.hpp"
#include "AtomData.hpp"
#include "GenericMoleculeData.hpp"
#include "MoleculeData.hpp"
#include "ReactionData.hpp"


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
const ObjT& DataTable<KeyT1, KeyT2, ObjT>::at(const KeyT1 key) const
{
	return table.atKey1(key);
}

template<class KeyT1, class KeyT2, class ObjT>
const ObjT& DataTable<KeyT1, KeyT2, ObjT>::at(const KeyT2 key) const
{
	return table.atKey2(key);
}

template<class KeyT1, class KeyT2, class ObjT>
const std::vector<ObjT>& DataTable<KeyT1, KeyT2, ObjT>::data() const
{
	return table.data();
}

template<class KeyT1, class KeyT2, class ObjT>
const size_t DataTable<KeyT1, KeyT2, ObjT>::size() const
{
	return table.size();
}

template<class KeyT1, class KeyT2, class ObjT>
const ObjT& DataTable<KeyT1, KeyT2, ObjT>::operator[](const size_t idx) const
{
	return table[idx];
}

template<class KeyT1, class KeyT2, class ObjT>
ObjT& DataTable<KeyT1, KeyT2, ObjT>::operator[](const size_t idx)
{
	return table[idx];
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

template class DataTable<ComponentId, Symbol, AtomData>;
template class DataTable<MoleculeId, std::string, MoleculeData>;
template class DataTable<MoleculeId, std::string, GenericMoleculeData>;
template class DataTable<ReactionId, std::string, ReactionData>;