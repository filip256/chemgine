#pragma once

#include <vector>

/// <summary>
/// Allows querying, applying modifiers and casting vectors.
/// Operations added by where() are queued, and only applied when yield() or yieldSelect() is called.
/// </summary>
template<class ObjT>
class Query
{
private:
	using OperationT = typename bool (*)(ObjT&);

	const std::vector<ObjT>* const source;
	std::vector<OperationT> operations;

	bool runOperations(ObjT& obj) const;

public:
	Query() noexcept;
	Query(const std::vector<ObjT>& source) noexcept;
	Query(const Query&) = default;
	Query(Query&&) = default;

	Query<ObjT>& where(OperationT query);
	Query<ObjT>& where(const Query<ObjT>& other);

	std::vector<ObjT> yield() const;
	std::vector<ObjT> yield(const std::vector<ObjT>&) const;

	template<class ObjC>
	std::vector<ObjC> yieldSelect(ObjC(*selector)(const ObjT&)) const;
	template<class ObjC>
	std::vector<ObjC> yieldSelect(ObjC(*selector)(const ObjT&), const std::vector<ObjT>& source) const;
};


template<class ObjT>
Query<ObjT>::Query() noexcept :
	source(nullptr)
{}

template<class ObjT>
Query<ObjT>::Query(const std::vector<ObjT>& source) noexcept :
	source(&source)
{}

template<class ObjT>
Query<ObjT>& Query<ObjT>::where(OperationT query)
{
	operations.emplace_back(query);
	return *this;
}

template<class ObjT>
bool Query<ObjT>::runOperations(ObjT& obj) const
{
	for (size_t i = 0; i < operations.size(); ++i)
		if (operations[i](obj) == false)
			return false;
	return true;
}

template<class ObjT>
Query<ObjT>& Query<ObjT>::where(const Query<ObjT>& other)
{
	this->operations.reserve(this->operations.capacity() + other.operations.size());
	for (size_t i = 0; i < other.operations.size(); ++i)
		this->operations.emplace_back(other.operations[i]);
	return *this;
}

template<class ObjT>
std::vector<ObjT> Query<ObjT>::yield() const
{
	if (this->source == nullptr)
		return std::vector<ObjT>();

	return yield(*this->source);
}

template<class ObjT>
template<class ObjC>
std::vector<ObjC> Query<ObjT>::yieldSelect(ObjC(*selector)(const ObjT&)) const
{
	if (this->source == nullptr)
		return std::vector<ObjC>();

	return yieldSelect(selector, *this->source);
}

template<class ObjT>
std::vector<ObjT> Query<ObjT>::yield(const std::vector<ObjT>& source) const
{
	std::vector<ObjT> result;
	for (size_t i = 0; i < source.size(); ++i)
	{
		auto copy = source[i];
		if (runOperations(copy))
			result.emplace_back(std::move(copy));
	}
	return result;
}

template<class ObjT>
template<class ObjC>
std::vector<ObjC> Query<ObjT>::yieldSelect(ObjC(*selector)(const ObjT&), const std::vector<ObjT>& source) const
{
	std::vector<ObjC> result;
	for (size_t i = 0; i < source.size(); ++i)
	{
		auto copy = source[i];
		if (runOperations(copy))
			result.emplace_back(selector(copy));
	}
	return result;
}