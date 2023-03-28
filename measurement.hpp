#pragma once

namespace zaoly
{
	template <typename number>
	class measurement // Levels of measurement
	{
	public:
		measurement(number num = {}) : data(num) {}

		virtual number raw() const
		{
			return data;
		}

	protected:
		number data;
	};

#define ASSIGNMENT_OPERATOR(class_name)      \
class_name& operator=(const class_name& num) \
{                                            \
	data = num.data;                         \
	return *this;                            \
}                                            \
                                             \
class_name& operator=(const number& num)     \
{                                            \
	data = num;                              \
	return *this;                            \
}

	template <typename number>
	class nominal : public measurement<number> // Nominal data, only distinguishes types
	{
	public:
		ASSIGNMENT_OPERATOR(nominal)

		template <typename number>
		friend bool operator==(const nominal<number>& a, const nominal<number>& b);

		template <typename number>
		friend bool operator!=(const nominal<number>& a, const nominal<number>& b);

	protected:
		using measurement<number>::measurement;
		using measurement<number>::data;
	};

	template <typename number>
	bool operator==(const nominal<number>& a, const nominal<number>& b)
	{
		return a.data == b.data;
	}

	template <typename number>
	bool operator!=(const nominal<number>& a, const nominal<number>& b)
	{
		return a.data != b.data;
	}

	template <typename number>
	class ordinal : public nominal<number> // Ordinal data, only distinguishes orders
	{
	public:
		ASSIGNMENT_OPERATOR(ordinal)

		template <typename number>
		friend bool operator<(const ordinal<number>& a, const ordinal<number>& b);

		template <typename number>
		friend bool operator>(const ordinal<number>& a, const ordinal<number>& b);

		template <typename number>
		friend bool operator<=(const ordinal<number>& a, const ordinal<number>& b);

		template <typename number>
		friend bool operator>=(const ordinal<number>& a, const ordinal<number>& b);

	protected:
		using nominal<number>::nominal;
		using nominal<number>::data;
	};

	template <typename number>
	bool operator<(const ordinal<number>& a, const ordinal<number>& b)
	{
		return a.data < b.data;
	}

	template <typename number>
	bool operator>(const ordinal<number>& a, const ordinal<number>& b)
	{
		return a.data > b.data;
	}

	template <typename number>
	bool operator<=(const ordinal<number>& a, const ordinal<number>& b)
	{
		return a.data <= b.data;
	}

	template <typename number>
	bool operator>=(const ordinal<number>& a, const ordinal<number>& b)
	{
		return a.data >= b.data;
	}

	template <typename number, size_t _unit>
	class ratio;

	template <typename number, size_t _unit = 1>
	class interval : public ordinal<number> // Interval data, only distinguishes locations, where only the interval between two data makes sense
	{
	public:
		const size_t unit = _unit;

		ASSIGNMENT_OPERATOR(interval)

		template <typename number, size_t _unit>
		friend interval<number, _unit> operator+(const interval<number, _unit>& a, const ratio<number, _unit>& b);

		template <typename number, size_t _unit>
		friend interval<number, _unit> operator+(const ratio<number, _unit>& a, const interval<number, _unit>& b);

		template <typename number, size_t _unit>
		friend interval<number, _unit> operator-(const interval<number, _unit>& a, const ratio<number, _unit>& b);

		template <typename number, size_t _unit>
		friend interval<number, _unit> operator-(const ratio<number, _unit>& a, const interval<number, _unit>& b);

		template <typename number, size_t _unit>
		friend ratio<number, _unit> operator-(const interval<number, _unit>& a, const interval<number, _unit>& b);

		interval operator+()
		{
			return interval(data);
		}

		interval operator-()
		{
			return interval(-data);
		}

		interval& operator+=(const ratio<number, _unit>& num)
		{
			data += num.data;
			return *this;
		}

		interval& operator-=(const ratio<number, _unit>& num)
		{
			data -= num.data;
			return *this;
		}

		interval& operator++()
		{
			++data;
			return *this;
		}

		interval& operator--()
		{
			--data;
			return *this;
		}

		interval operator++(int)
		{
			interval result(*this);
			data++;
			return result;
		}

		interval operator--(int)
		{
			interval result(*this);
			data--;
			return result;
		}

	protected:
		using ordinal<number>::ordinal;
		using ordinal<number>::data;
	};

	template <typename number, size_t _unit = 1>
	class ratio : public interval<number, _unit> // Ratio data, can perform any operation
	{
	public:
		const size_t unit = _unit;

		ASSIGNMENT_OPERATOR(ratio)

		// With interval data:

		template <typename number, size_t _unit>
		friend interval<number, _unit> operator+(const interval<number, _unit>& a, const ratio<number, _unit>& b);

		template <typename number, size_t _unit>
		friend interval<number, _unit> operator+(const ratio<number, _unit>& a, const interval<number, _unit>& b);

		template <typename number, size_t _unit>
		friend interval<number, _unit> operator-(const interval<number, _unit>& a, const ratio<number, _unit>& b);

		template <typename number, size_t _unit>
		friend interval<number, _unit> operator-(const ratio<number, _unit>& a, const interval<number, _unit>& b);

		template <typename number, size_t _unit>
		friend ratio<number, _unit> operator-(const interval<number, _unit>& a, const interval<number, _unit>& b);

		template <typename number, size_t _unit>
		friend interval<number, _unit>& interval<number, _unit>::operator+=(const ratio<number, _unit>& num);

		template <typename number, size_t _unit>
		friend interval<number, _unit>& interval<number, _unit>::operator-=(const ratio<number, _unit>& num);

		// Without interval data:

		template <typename number, size_t _unit>
		friend ratio<number, _unit> operator+(const ratio<number, _unit>& a, const ratio<number, _unit>& b);

		template <typename number, size_t _unit>
		friend ratio<number, _unit> operator-(const ratio<number, _unit>& a, const ratio<number, _unit>& b);

		template <typename number, size_t _unit_1, size_t _unit_2>
		friend ratio<number, _unit_1 * _unit_2> operator*(const ratio<number, _unit_1>& a, const ratio<number, _unit_2>& b);

		template <typename number, size_t _unit_1, size_t _unit_2>
		friend ratio<number, _unit_1 / _unit_2> operator/(const ratio<number, _unit_1>& a, const ratio<number, _unit_2>& b);

		template <typename number, size_t _unit_1, size_t _unit_2>
		friend ratio<number, _unit_1> operator%(const ratio<number, _unit_1>& a, const ratio<number, _unit_2>& b);

		ratio& operator*=(const ratio& num)
		{
			data *= num.data;
			return *this;
		}

		ratio& operator/=(const ratio& num)
		{
			data /= num.data;
			return *this;
		}

		ratio& operator%=(const ratio& num)
		{
			data %= num.data;
			return *this;
		}

		ratio& operator++()
		{
			++data;
			return *this;
		}

		ratio& operator--()
		{
			--data;
			return *this;
		}

		ratio operator++(int)
		{
			ratio result(*this);
			data++;
			return result;
		}

		ratio operator--(int)
		{
			ratio result(this);
			data--;
			return result;
		}

	protected:
		using interval<number, _unit>::interval;
		using interval<number, _unit>::data;
	};

	// Operators with interval data, maybe also ratio data:

	template <typename number, size_t _unit>
	interval<number, _unit> operator+(const interval<number, _unit>& a, const ratio<number, _unit>& b)
	{
		return interval<number, _unit>(a.data + b.data);
	}

	template <typename number, size_t _unit>
	interval<number, _unit> operator+(const ratio<number, _unit>& a, const interval<number, _unit>& b)
	{
		return interval<number, _unit>(a.data + b.data);
	}

	template <typename number, size_t _unit>
	interval<number, _unit> operator-(const interval<number, _unit>& a, const ratio<number, _unit>& b)
	{
		return interval<number, _unit>(a.data - b.data);
	}

	template <typename number, size_t _unit>
	interval<number, _unit> operator-(const ratio<number, _unit>& a, const interval<number, _unit>& b)
	{
		return interval<number, _unit>(a.data - b.data);
	}

	template <typename number, size_t _unit>
	ratio<number, _unit> operator-(const interval<number, _unit>& a, const interval<number, _unit>& b)
	{
		return ratio<number, _unit>(a.data - b.data);
	}

	// Functions with only ratio data:

	template <typename number, size_t _unit>
	ratio<number, _unit> operator+(const ratio<number, _unit>& a, const ratio<number, _unit>& b)
	{
		return ratio<number, _unit>(a.data + b.data);
	}

	template <typename number, size_t _unit>
	ratio<number, _unit> operator-(const ratio<number, _unit>& a, const ratio<number, _unit>& b)
	{
		return ratio<number, _unit>(a.data - b.data);
	}

	template <typename number, size_t _unit_1, size_t _unit_2>
	ratio<number, _unit_1 * _unit_2> operator*(const ratio<number, _unit_1>& a, const ratio<number, _unit_2>& b)
	{
		return ratio<number, _unit_1 * _unit_2>(a.data * b.data);
	}

	template <typename number, size_t _unit_1, size_t _unit_2>
	ratio<number, _unit_1 / _unit_2> operator/(const ratio<number, _unit_1>& a, const ratio<number, _unit_2>& b)
	{
		static_assert(_unit_2 > 0 && _unit_1 % _unit_2 == 0, "Unit 1 is indivisible by Unit 2");
		return ratio<number, _unit_1 / _unit_2>(a.data / b.data);
	}

	template <typename number, size_t _unit_1, size_t _unit_2>
	ratio<number, _unit_1> operator%(const ratio<number, _unit_1>& a, const ratio<number, _unit_2>& b)
	{
		return ratio<number, _unit_1>(a.data % b.data);
	}
}
