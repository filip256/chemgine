#pragma once

#include <cstdint>

template<class ObjT>
class Result
{
public:
	int8_t status;
	ObjT result;

public:
	Result();
	Result(ObjT&& object, const int8_t status = 1);
	Result(Result&& other);
};
