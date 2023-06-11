#include "Result.hpp"

#include <utility>


template <typename ObjT>
Result<ObjT>::Result() :
	result(),
	status(0)
{}

template <class ObjT>
Result<ObjT>::Result(ObjT&& result, const int8_t status) :
	result(result),
	status(status)
{}

template <class ObjT>
Result<ObjT>::Result(Result&& other) :
	result(std::move(other.result)),
	status(other.status)
{}

template class Result<int>;
template class Result<unsigned int>;
template class Result<double>;