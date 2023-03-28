#pragma once

#include <iostream>

namespace zaoly
{
	class matrix_unaligned : std::logic_error
	{
	public:
		matrix_unaligned() : std::logic_error("Matrix unaligned") {}
	};

	class matrix_too_small : std::logic_error
	{
	public:
		matrix_too_small() : std::logic_error("Matrix too small") {}
	};

	class subscript_out_of_range : std::logic_error
	{
	public:
		subscript_out_of_range() : std::logic_error("Subscript out of range") {}
	};

	template <typename number>
	class matrix
	{
		using row_type    = std::initializer_list<number>;
		using matrix_type = std::initializer_list<row_type>;

	public:
		matrix(size_t row_count, size_t column_count, const matrix_type& nums = {}) :
			_row_count(row_count), _column_count(column_count)
		{
			if (row_count <= 0 || column_count <= 0)
				throw matrix_too_small();
			_data = new number[_row_count * _column_count]{};
			if (nums.begin() == nullptr)
				return;
			for (size_t row_index = 0; row_index < nums.size() && row_index < _row_count; ++row_index)
				for (size_t column_index = 0; column_index < nums.begin()[row_index].size() && column_index < _column_count; ++column_index)
					_data[row_index * _column_count + column_index] = nums.begin()[row_index].begin()[column_index];
		}

		matrix(const matrix_type& nums = {})
		{
			if (nums.begin() == nullptr || nums.size() == 0)
				throw matrix_too_small();
			_row_count = nums.size();
			_column_count = nums.begin()->size();
			for (size_t row_index = 1; row_index < _row_count; ++row_index)
				if (nums.begin()[row_index].size() != _column_count)
					throw matrix_unaligned();
			_data = new number[_row_count * _column_count];
			for (size_t row_index = 0; row_index < _row_count; ++row_index)
				for (size_t column_index = 0; column_index < _column_count; ++column_index)
					_data[row_index * _column_count + column_index] = nums.begin()[row_index].begin()[column_index];
		}

		matrix(const matrix& matr)
		{
			copy_assign(matr);
		}

		matrix(matrix&& matr) noexcept
		{
			move_assign(std::move(matr));
		}

		~matrix()
		{
			delete[] _data;
		}

		size_t row_count() const
		{
			return _row_count;
		}

		size_t column_count() const
		{
			return _column_count;
		}

		void resize(size_t new_row_count, size_t new_column_count)
		{
			if (new_row_count <= 0 || new_column_count <= 0)
				throw matrix_too_small();
			matrix result(new_row_count, new_column_count);
			for (size_t row_index = 0; row_index < _row_count && row_index < new_row_count; ++row_index)
				for (size_t column_index = 0; column_index < _column_count && column_index < new_column_count; ++column_index)
					result.data(row_index, column_index) = data(row_index, column_index);
			*this = std::move(result);
		}

		number* data()
		{
			return _data;
		}

		const number* data() const
		{
			return _data;
		}

		number& data(size_t row_index, size_t column_index)
		{
			if (row_index >= _row_count || column_index >= _column_count)
				throw subscript_out_of_range();
			else
				return _data[row_index * _column_count + column_index];
		}

		const number& data(size_t row_index, size_t column_index) const
		{
			if (row_index >= _row_count || column_index >= _column_count)
				throw subscript_out_of_range();
			else
				return _data[row_index * _column_count + column_index];
		}

		matrix transpose() const
		{
			matrix result(_column_count, _row_count);
			for (size_t row_index = 0; row_index < _row_count; ++row_index)
				for (size_t column_index = 0; column_index < _column_count; ++column_index)
					result.data(column_index, row_index) = data(row_index, column_index);
			return result;
		}

		matrix minor(size_t row_index, size_t column_index) const
		{
			matrix result(_row_count - 1, _column_count - 1);
			size_t new_row_index{}, new_column_index{};
			for (size_t old_row_index = 0; old_row_index < _row_count; ++old_row_index)
			{
				if (old_row_index == row_index)
					continue;
				new_column_index = 0;
				for (size_t old_column_index = 0; old_column_index < _column_count; ++old_column_index)
				{
					if (old_column_index == column_index)
						continue;
					result.data(new_row_index, new_column_index) = data(old_row_index, old_column_index);
					++new_column_index;
				}
				++new_row_index;
			}
			return result;
		}

		number cofactor(size_t row_index, size_t column_index) const
		{
			if (row_index % 2 == column_index % 2)
				return minor(row_index, column_index).determinant();
			else
				return -minor(row_index, column_index).determinant();
		}

		number determinant() const
		{
			number single_element{}, result{};
			if (_row_count != _column_count)
				throw matrix_unaligned();
			else if (_row_count == 1)
				return data(0, 0);
			for (size_t row_index = 0; row_index < _row_count; ++row_index)
			{
				single_element = data(row_index, 0);
				if (single_element != 0.0)
					result += single_element * cofactor(row_index, 0);
			}
			return result;
		}

		matrix adjoint() const
		{
			matrix result(_row_count, _column_count);
			for (size_t row_index = 0; row_index < _row_count; ++row_index)
				for (size_t column_index = 0; column_index < _column_count; ++column_index)
					result.data(row_index, column_index) = cofactor(row_index, column_index);
			return result;
		}

		matrix adjoint_transpose() const
		{
			return transpose().adjoint();
		}

		bool invertible() const
		{
			return determinant() != 0.0;
		}

		matrix inverse() const
		{
			matrix result = adjoint_transpose();
			number det = determinant();
			for (size_t row_index = 0; row_index < _row_count; ++row_index)
				for (size_t column_index = 0; column_index < _column_count; ++column_index)
					result.data(row_index, column_index) /= det;
			return result;
		}

		void print(std::ostream& os = std::cout, const char* delimeter1 = " ", const char* delimeter2 = "\n") const
		{
			for (size_t row_index = 0; row_index < _row_count; ++row_index)
			{
				if (row_index > 0)
					os << delimeter2;
				for (size_t column_index = 0; column_index < _column_count; ++column_index)
				{
					if (column_index > 0)
						os << delimeter1;
					os << data(row_index, column_index);
				}
			}
		}

		template <typename number>
		friend matrix<number> operator+(const matrix<number>& a, const matrix<number>& b);

		template <typename number>
		friend matrix<number> operator-(const matrix<number>& a, const matrix<number>& b);

		template <typename number>
		friend matrix<number> operator*(const matrix<number>& a, const matrix<number>& b);

		template <typename number>
		friend matrix<number> operator/(const matrix<number>& a, const matrix<number>& b);

		matrix& operator=(const matrix& matr)
		{
			copy_assign(matr);
			return *this;
		}

		matrix& operator=(matrix&& matr) noexcept
		{
			move_assign(std::move(matr));
			return *this;
		}

		number* operator[](size_t row_index)
		{
			return _data + row_index * _column_count;
		}

		const number* operator[](size_t row_index) const
		{
			return _data + row_index * _column_count;
		}

		number& operator()(size_t row_index, size_t column_index)
		{
			return data(row_index, column_index);
		}

		const number& operator()(size_t row_index, size_t column_index) const
		{
			return data(row_index, column_index);
		}

	private:
		size_t _row_count, _column_count;
		number* _data;

		void copy_assign(const matrix& matr)
		{
			_row_count = matr._row_count;
			_column_count = matr._column_count;
			for (size_t row_index = 0; row_index < _row_count; ++row_index)
				for (size_t column_index = 0; column_index < _column_count; ++column_index)
					_data[row_index * _column_count + column_index] = matr._data[row_index * _column_count + column_index];
		}

		void move_assign(matrix&& matr) noexcept
		{
			_row_count = matr._row_count;
			_column_count = matr._column_count;
			std::swap(_data, matr._data);
		}
	};

	template <typename number>
	matrix<number> operator+(const matrix<number>& a, const matrix<number>& b)
	{
		if (a._row_count != b._row_count || a._column_count != b._column_count)
			throw matrix_unaligned();
		matrix<number> result(a._row_count, a._column_count);
		for (size_t row_index = 0; row_index < result._row_count; ++row_index)
			for (size_t column_index = 0; column_index < result._column_count; ++column_index)
				result.data(row_index, column_index) = a.data(row_index, column_index) + b.data(row_index, column_index);
		return result;
	}

	template <typename number>
	matrix<number> operator-(const matrix<number>& a, const matrix<number>& b)
	{
		if (a._row_count != b._row_count || a._column_count != b._column_count)
			throw matrix_unaligned();
		matrix<number> result(a._row_count, a._column_count);
		for (size_t row_index = 0; row_index < result._row_count; ++row_index)
			for (size_t column_index = 0; column_index < result._column_count; ++column_index)
				result.data(row_index, column_index) = a.data(row_index, column_index) - b.data(row_index, column_index);
		return result;
	}

	template <typename number>
	matrix<number> operator*(const matrix<number>& a, const matrix<number>& b)
	{
		if (a._column_count != b._row_count)
			throw matrix_unaligned();
		matrix<number> result(a._row_count, b._column_count);
		for (size_t row_index = 0; row_index < result._row_count; ++row_index)
			for (size_t column_index = 0; column_index < result._column_count; ++column_index)
				for (size_t mid_index = 0; mid_index < a._column_count; ++mid_index)
					result.data(row_index, column_index) += a.data(row_index, mid_index) * b.data(mid_index, column_index);
		return result;
	}

	template <typename number>
	matrix<number> operator/(const matrix<number>& a, const matrix<number>& b)
	{
		return a * b.inverse();
	}

	template <typename number>
	std::ostream& operator<<(std::ostream& os, const matrix<number>& matr)
	{
		matr.print(os);
		return os;
	}
}
