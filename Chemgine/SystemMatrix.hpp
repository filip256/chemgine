#pragma once

#include <vector>

template <class T>
class SystemMatrix
{
private:
	std::vector<std::vector<T>> matrix;

	bool toREF();

public:
	SystemMatrix() = default;
	SystemMatrix(std::initializer_list<std::initializer_list<T>> initializer) noexcept;
	SystemMatrix(const SystemMatrix&) = delete;

	void addRow(std::vector<T>&& row, const T result);
	void addNullRow(const size_t n);
	void swapRows(const size_t x, const size_t y);

	bool isNullRow(const size_t idx);

	bool trySolution(const std::vector<T>& solution) const;

	const std::vector<T>& operator[](const size_t idx) const;
	std::vector<T>& operator[](const size_t idx);
	std::vector<T>& back();

	std::vector<T> solve();
};