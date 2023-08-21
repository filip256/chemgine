#pragma once

#include <vector>
#include <memory>

/// <summary>
/// Vector of pointers to dynamically allocated objects, handles its own memory.
/// </summary>
template <class ObjT, class sizeT = uint32_t>
class PVector
{
private:
	std::vector<const ObjT*> data;

public:
	PVector(const std::allocator<ObjT>& alloc = std::allocator<ObjT>()) noexcept;
	PVector(const PVector<ObjT, sizeT>&) = delete;
	PVector(PVector<ObjT, sizeT>&& other) = default;
	~PVector() noexcept;

	void push_back(const ObjT* obj);
	void push_back(ObjT&& obj);
	void pop_back();
	void insert(const ObjT* obj, const sizeT idx);
	void insert(ObjT&& obj, const sizeT idx);
	void erase(const sizeT idx);
	void erase(const sizeT first, const sizeT last);

	sizeT size() const;
	bool empty() const;
	sizeT capacity() const;
	void reserve(const sizeT newCap);
	void clear();

	const ObjT& front() const;
	ObjT& front();
	const ObjT& back() const;
	ObjT& back();
	const ObjT& operator[](const sizeT idx) const;
	ObjT& operator[](const sizeT idx);

	sizeT findFirst(bool (*predicate) (const ObjT&)) const;

	static constexpr sizeT npos = static_cast<sizeT>(-1);
};


template<class ObjT, class sizeT>
PVector<ObjT, sizeT>::PVector(const std::allocator<ObjT>& alloc) noexcept :
	data(alloc)
{}

template<class ObjT, class sizeT>
PVector<ObjT, sizeT>::~PVector() noexcept
{
	clear();
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::push_back(const ObjT* obj)
{
	data.push_back(obj);
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::push_back(ObjT&& obj)
{
	data.push_back(new ObjT(obj));
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::pop_back()
{
	delete data.back();
	data.pop_back();
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::insert(const ObjT* obj, const sizeT idx)
{
	data.insert(data.begin() + idx, obj);
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::insert(ObjT&& obj, const sizeT idx)
{
	data.insert(data.begin() + idx, new ObjT(obj));
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::erase(const sizeT idx)
{
	data.erase(data.begin() + idx);
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::erase(const sizeT first, const sizeT last)
{
	data.insert(data.begin() + first, data.begin() + last);
}

template<class ObjT, class sizeT>
sizeT PVector<ObjT, sizeT>::size() const
{
	return data.size();
}

template<class ObjT, class sizeT>
bool PVector<ObjT, sizeT>::empty() const
{
	return data.empty();
}

template<class ObjT, class sizeT>
sizeT PVector<ObjT, sizeT>::capacity() const
{
	return data.capacity();
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::reserve(const sizeT newCap)
{
	return data.reserve(newCap);
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::clear()
{
	while (data.size())
	{
		delete data.back();
		data.pop_back();
	}
}

template<class ObjT, class sizeT>
const ObjT& PVector<ObjT, sizeT>::front() const
{
	return *data.back();
}

template<class ObjT, class sizeT>
ObjT& PVector<ObjT, sizeT>::front()
{
	return *data.back();
}

template<class ObjT, class sizeT>
const ObjT& PVector<ObjT, sizeT>::back() const
{
	return *data.back();
}

template<class ObjT, class sizeT>
ObjT& PVector<ObjT, sizeT>::back()
{
	return *data.back();
}

template<class ObjT, class sizeT>
const ObjT& PVector<ObjT, sizeT>::operator[](const sizeT idx) const
{
	return *data[idx];
}

template<class ObjT, class sizeT>
ObjT& PVector<ObjT, sizeT>::operator[](const sizeT idx)
{
	return *data[idx];
}

template<class ObjT, class sizeT>
sizeT PVector<ObjT, sizeT>::findFirst(bool (*predicate) (const ObjT&)) const
{
	for (sizeT i = 0; i < data.size(); ++i)
		if (predicate(data[i]))
			return i;
	return npos;
}
