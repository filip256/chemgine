#include "SystemMatrix.hpp"


template <class T>
bool SystemMatrix<T>::toREF()
{
    for (size_t k = 0; k < matrix.size(); ++k)
    {
        // Initialize maximum value and index for pivot
        size_t i_max = k;
        size_t v_max = matrix[i_max][k];

        /* find greater amplitude for pivot if any */
        for (size_t i = k + 1; i < matrix.size(); ++i)
            if (std::abs(matrix[i][k]) > v_max)
                v_max = matrix[i][k], i_max = i;

        /* if a principal diagonal element  is zero,
         * it denotes that matrix is singular, and
         * will lead to a division-by-zero later. */
        if (matrix[k][i_max] == 0)
            return false; // Matrix is singular

        /* Swap the greatest value row with current row */
        if (i_max != k)
            swapRows(k, i_max);


        for (size_t i = k + 1; i < matrix.size(); ++i)
        {
            /* factor f to set current row kth element to 0,
             * and subsequently remaining kth column to 0 */
            double f = matrix[i][k] / matrix[k][k];

            /* subtract fth multiple of corresponding kth
               row element*/
            for (size_t j = k + 1; j <= matrix.size(); ++j)
                matrix[i][j] -= matrix[k][j] * f;

            /* filling lower triangular matrix with zeros*/
            matrix[i][k] = 0;
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

    std::vector<T> result(matrix.size() - 1, 0);  // An array to store solution

    /* Start calculating from last equation up to the
       first */
    for (int i = matrix.size() - 1; i >= 0; --i)
    {
        /* start with the RHS of the equation */
        result[i] = matrix[i].back();

        /* Initialize j to i+1 since matrix is upper
           triangular*/
        for (size_t j = i + 1; j < matrix.size(); ++j)
        {
            /* subtract all the lhs values
             * except the coefficient of the variable
             * whose value is being calculated */
            result[i] -= matrix[i][j] * result[j];
        }

        /* divide the RHS by the coefficient of the
           unknown being calculated */
        result[i] = result[i] / matrix[i][i];
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

template class SystemMatrix<double>;
template class SystemMatrix<float>;