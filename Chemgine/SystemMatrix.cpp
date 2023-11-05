#include "SystemMatrix.hpp"

#include <iostream>

template <class T>
bool SystemMatrix<T>::toREF()
{
    size_t cols = matrix[0].size() - 1;
    size_t rows = cols;

    for (size_t i = 0; i < rows; ++i) 
    {
        size_t pivotRow = i;
        for (size_t k = i + 1; k < rows; ++k) {
            if (std::abs(matrix[k][i]) > std::abs(matrix[pivotRow][i])) {
                pivotRow = k;
            }
        }

        if (pivotRow != i)
            swapRows(i, pivotRow);

        const T pivot = matrix[i][i];
        if (std::abs(matrix[i][i]) < 1e-10)
            return false;

        for (size_t j = i; j < cols + 1; ++j)
            matrix[i][j] /= pivot;

        // Eliminate non-zero elements below the pivot
        for (size_t k = i + 1; k < rows; ++k) 
        {
            const T factor = matrix[k][i];
            for (size_t j = i; j < cols + 1; ++j) 
                matrix[k][j] -= factor * matrix[i][j];
        }
    }

    return true;
}

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
void SystemMatrix<T>::swapRows(const size_t x, const size_t y)
{
    matrix[x].swap(matrix[y]);
}


template <class T>
std::vector<T> SystemMatrix<T>::solve()
{
    if(toREF() == false)
        return std::vector<T>();

    size_t n = matrix[0].size() - 1;
    std::vector<T> result(n, 0);

    for (size_t i = n; i-- > 0;) 
    {
        result[i] = matrix[i][n];
        for (size_t j = i + 1; j < n; ++j)
            result[i] -= matrix[i][j] * result[j];
        result[i] /= matrix[i][i];
    }

	return result;
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
template class SystemMatrix<double>;
template class SystemMatrix<long double>;