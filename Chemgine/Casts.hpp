#pragma once

#include<typeinfo>

/// <summary>
/// Performs a typeid-checked cast. Cheaper than dynamic_cast but can only downcast to final types.
/// </summary>
template<typename DstT, typename SrcT>
DstT* final_cast(SrcT& src)
{
	return typeid(src) == typeid(DstT) ?
		static_cast<DstT*>(&src) :
		nullptr;
}

/// <summary>
/// Performs a typeid-checked cast. Cheaper than dynamic_cast but can only downcast to final types.
/// </summary>
template<typename DstT, typename SrcT>
const DstT* final_cast(const SrcT& src)
{
	return typeid(src) == typeid(DstT) ?
		static_cast<const DstT*>(&src) :
		nullptr;
}
