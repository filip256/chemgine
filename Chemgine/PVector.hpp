#pragma once

#include <vector>
#include <memory>

/// <summary>
/// Vector of pointers to dynamically allocated objects, handles its own memory.
/// If ObjT is a constant type then the allocated ObjT objects maintain the const qualifier.
/// NOTE: there is an overhead for using PVector
/// </summary>
template <class ObjT, class sizeT = uint32_t>
class PVector
{
private:
	using PObjT = typename std::conditional<std::is_const<ObjT>::value, const ObjT*, ObjT*>::type;
	std::vector<PObjT> content;

public:
	PVector(const std::allocator<typename std::remove_const<ObjT>::type>& alloc = std::allocator<typename std::remove_const<ObjT>::type>()) noexcept;
	PVector(const PVector<ObjT, sizeT>&) = delete;
	PVector(PVector<ObjT, sizeT>&& other) = default;
	PVector(std::vector<PObjT>&& other) noexcept;
	~PVector() noexcept;

	/// <summary>
	/// Does not deallocate the old content. Call clear() before assignemnt if deallocation is intended.
	/// </summary>
	PVector<ObjT, sizeT>& operator=(PVector<ObjT, sizeT>&& other) = default;
	/// <summary>
	/// Does not deallocate the old content. Call clear() before assignemnt if deallocation is intended.
	/// </summary>
	PVector<ObjT, sizeT>& operator=(std::vector<PObjT>&& other) noexcept;

	void emplace_back(PObjT obj);
	void pop_back();
	void insert(PObjT obj, const sizeT idx);
	void insert(ObjT&& obj, const sizeT idx);
	PObjT release(const sizeT idx);
	PObjT release_back();
	void erase(const sizeT idx);
	void erase(const sizeT first, const sizeT last);

	sizeT size() const;
	bool empty() const;
	sizeT capacity() const;
	void reserve(const sizeT newCap);
	void clear();
	void release();

	const std::vector<PObjT>& data() const;
	std::vector<PObjT>& data();
	PObjT front() const;
	ObjT* front();
	PObjT back() const;
	ObjT* back();
	PObjT operator[](const sizeT idx) const;
	ObjT*& operator[](const sizeT idx);

	sizeT findFirst(bool (*predicate) (const ObjT&)) const;

	static constexpr sizeT npos = static_cast<sizeT>(-1);
};


template<class ObjT, class sizeT>
PVector<ObjT, sizeT>::PVector(const std::allocator<typename std::remove_const<ObjT>::type>& alloc) noexcept :
	content(alloc)
{}

template<class ObjT, class sizeT>
PVector<ObjT, sizeT>::PVector(std::vector<PObjT>&& other) noexcept :
	content(std::move(other))
{}

template<class ObjT, class sizeT>
PVector<ObjT, sizeT>::~PVector() noexcept
{
	clear();
}

template<class ObjT, class sizeT>
PVector<ObjT, sizeT>& PVector<ObjT, sizeT>::operator=(std::vector<PObjT>&& other) noexcept
{
	if (&this->content == &other)
		return *this;

	this->content = std::move(other);
	return *this;
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::emplace_back(PObjT obj)
{
	content.push_back(obj);
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::pop_back()
{
	delete content.back();
	content.pop_back();
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::insert(PObjT obj, const sizeT idx)
{
	content.insert(content.begin() + idx, obj);
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::insert(ObjT&& obj, const sizeT idx)
{
	content.insert(content.begin() + idx, new ObjT(obj));
}

template<class ObjT, class sizeT>
PVector<ObjT, sizeT>::PObjT PVector<ObjT, sizeT>::release(const sizeT idx)
{
	const auto temp = content[idx];
	content.erase(content.begin() + idx);
	return temp;
}

template<class ObjT, class sizeT>
PVector<ObjT, sizeT>::PObjT PVector<ObjT, sizeT>::release_back()
{
	const auto temp = content.back();
	content.pop_back();
	return temp;
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::erase(const sizeT idx)
{
	delete content[idx];
	content.erase(content.begin() + idx);
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::erase(const sizeT first, const sizeT last)
{
	for (sizeT i = first; i < last; ++i)
		delete content[i];

	content.erase(content.begin() + first, content.begin() + last);
}

template<class ObjT, class sizeT>
sizeT PVector<ObjT, sizeT>::size() const
{
	return content.size();
}

template<class ObjT, class sizeT>
bool PVector<ObjT, sizeT>::empty() const
{
	return content.empty();
}

template<class ObjT, class sizeT>
sizeT PVector<ObjT, sizeT>::capacity() const
{
	return content.capacity();
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::reserve(const sizeT newCap)
{
	return content.reserve(newCap);
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::clear()
{
	while (content.size())
	{
		delete content.back();
		content.pop_back();
	}
}

template<class ObjT, class sizeT>
void PVector<ObjT, sizeT>::release()
{
	content.clear();
}

template<class ObjT, class sizeT>
const std::vector<typename PVector<ObjT, sizeT>::PObjT>& PVector<ObjT, sizeT>::data() const
{
	return content;
}

template<class ObjT, class sizeT>
std::vector<typename PVector<ObjT, sizeT>::PObjT>& PVector<ObjT, sizeT>::data()
{
	return content;
}

template<class ObjT, class sizeT>
typename PVector<ObjT, sizeT>::PObjT PVector<ObjT, sizeT>::front() const
{
	return content.back();
}

template<class ObjT, class sizeT>
ObjT* PVector<ObjT, sizeT>::front()
{
	return content.back();
}

template<class ObjT, class sizeT>
typename PVector<ObjT, sizeT>::PObjT PVector<ObjT, sizeT>::back() const
{
	return content.back();
}

template<class ObjT, class sizeT>
ObjT* PVector<ObjT, sizeT>::back()
{
	return content.back();
}

template<class ObjT, class sizeT>
typename PVector<ObjT, sizeT>::PObjT PVector<ObjT, sizeT>::operator[](const sizeT idx) const
{
	return content[idx];
}

template<class ObjT, class sizeT>
ObjT*& PVector<ObjT, sizeT>::operator[](const sizeT idx)
{
	return content[idx];
}

template<class ObjT, class sizeT>
sizeT PVector<ObjT, sizeT>::findFirst(bool (*predicate) (const ObjT&)) const
{
	for (sizeT i = 0; i < content.size(); ++i)
		if (predicate(content[i]))
			return i;
	return npos;
}
