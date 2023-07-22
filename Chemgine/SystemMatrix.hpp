#pragma once

#include <vector>

template <class T>
class SystemMatrix
{
private:
	std::vector<std::vector<T>> matrix;

public:
	SystemMatrix() = default;
	SystemMatrix(const SystemMatrix&) = delete;

	void addRow(std::vector<T>&& row, const T result);
	void addRow(const size_t n);

	const std::vector<T>& operator[](const size_t idx) const;
	std::vector<T>& operator[](const size_t idx);
	std::vector<T>& back();

	std::vector<T> solve();
};