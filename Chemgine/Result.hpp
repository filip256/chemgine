#pragma once

#include <cstdint>
#include <utility>

template<class ObjT>
class Result
{
public:
	int8_t status;
	ObjT result;

public:
	Result();
	Result(ObjT&& object, const int8_t status = 1) noexcept;
	Result(const ObjT& object, const int8_t status = 1) noexcept;
	Result(Result&& other) noexcept;
};



template <typename ObjT>
Result<ObjT>::Result() :
	result(),
	status(0)
{}

template <class ObjT>
Result<ObjT>::Result(ObjT&& result, const int8_t status) noexcept :
	result(std::move(result)),
	status(status)
{}

template <class ObjT>
Result<ObjT>::Result(const ObjT& result, const int8_t status) noexcept :
	result(result),
	status(status)
{}

template <class ObjT>
Result<ObjT>::Result(Result&& other) noexcept :
	result(std::move(other.result)),
	status(other.status)
{}