#include "SystemMatrix.hpp"


template <class T>
void SystemMatrix<T>::addRow(std::vector<T>&& row, const T result)
{
	matrix.emplace_back(std::move(row));
	matrix.back().emplace_back(result);
}

template <class T>
void SystemMatrix<T>::addRow(const size_t n)
{
	matrix.emplace_back(std::move(std::vector<T>(n + 1, 0)));
}

template <class T>
std::vector<T> SystemMatrix<T>::solve()
{
	return std::vector<T>();
}

template <class T>
const std::vector<T>& SystemMatrix<T>::operator[](const size_t idx) const
{
	return matrix[idx];
}

template <class T>
std::vector<T>& SystemMatrix<T>::operator[](const size_t idx)
{
	return matrix[idx];
}

template <class T>
std::vector<T>& SystemMatrix<T>::back()
{
	return matrix.back();
}

template class SystemMatrix<float>;
template class SystemMatrix<int>;