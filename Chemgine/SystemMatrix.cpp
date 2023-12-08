#include "SystemMatrix.hpp"

#include <iostream>

template<class T>
SystemMatrix<T>::SystemMatrix(std::initializer_list<std::initializer_list<T>> initializer) noexcept :
    matrix(initializer.begin(), initializer.end())
{}

template <class T>
bool SystemMatrix<T>::toREF()
{
    size_t cols = matrix[0].size();
    size_t rows = matrix.size();

    for (size_t i = 0; i < rows - 1; ++i) 
    {
        size_t pivotRow = i;
        for (size_t k = i + 1; k < rows; ++k)
            if (std::abs(matrix[k][i]) > std::abs(matrix[pivotRow][i])) 
                pivotRow = k;

        if (pivotRow != i)
            swapRows(i, pivotRow);

        const auto pivot = matrix[i][i];
        if (std::abs(pivot) < 1e-10)
            return false;

        for (size_t j = i; j < cols; ++j)
            matrix[i][j] /= pivot;

        // eliminate non-zero elements below the pivot
        for (size_t k = i + 1; k < rows; ++k) 
        {
            const auto factor = matrix[k][i];
            for (size_t j = i; j < cols; ++j) 
                matrix[k][j] -= factor * matrix[i][j];
        }
    }

    // NxN systems can still be solved if one row is redundant (otherwise no solution)
    // If such a row exists it will be bubbled to the last row and it will be null
    // The whole last row is null if the pivot is null (in NxN systems)
    if (cols == rows)
    {
        if (std::abs(matrix.back().back()) < 1e-7) // 1e-7 could introduce errors in the solution but it allows solving more systems
        {
            matrix.pop_back();
            return true;
        }
        return false;
    }

    // last row
    const auto pivot = matrix.back()[rows - 1];
    if (std::abs(pivot) < 1e-10)
        return false;

    for (size_t j = rows - 1; j < cols; ++j)
        matrix.back()[j] /= pivot;

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
bool SystemMatrix<T>::trySolution(const std::vector<T>& solution) const
{
    for (size_t i = 0; i < matrix.size(); ++i)
    {
        T temp = 0;
        for (size_t j = 0; j < matrix[i].size() - 1; ++j)
            temp += matrix[i][j] * solution[j];

        if (std::abs(temp - matrix[i].back()) > 1e-10)
            return false;
    }
    return true;
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