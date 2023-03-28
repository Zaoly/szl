#pragma once

#include "../zaoly-numlist/numlist.hpp"

namespace zaoly
{
	template <typename data_type>
	class combination : public numlist<data_type>
	{
		using numlist<data_type>::nl_n;
		using numlist<data_type>::nl_data;
		using numlist<data_type>::nl_delimeter;

	public:
		combination(unsigned n, data_type min, data_type max, const char* delimeter = " ") :
			numlist<data_type>::numlist(n, delimeter),
			c_min(min),
			c_max(max)
		{
			if (unsigned(max - min + 1) < n)
				throw std::range_error("Number range too narrow");
			reset();
		}

		combination(const combination& num) : numlist<data_type>::numlist(num.nl_n, num.nl_delimeter)
		{
			*this = num;
		}

		combination(combination&& num) noexcept : numlist<data_type>::numlist(num.nl_n, num.nl_delimeter)
		{
			*this = std::move(num);
		}

		void next() // The data at subscript 0 is the easiest to change. Note that the combination is stored in reverse order!
		{
			unsigned pos = 0;
			for (; pos < nl_n; ++pos)
				if (nl_data[pos] != c_max - pos)
					break;
			if (pos < nl_n)
			{
				++nl_data[pos];
				for (unsigned i = 0; i < pos; ++i)
					nl_data[i] = nl_data[pos] + (pos - i);
			}
			else
				reset();
		}

		void previous()
		{
			unsigned pos = 0;
			for (; pos < nl_n - 1; ++pos)
				if (nl_data[pos] != nl_data[pos + 1] + 1)
					break;
			if (pos < nl_n - 1 || nl_data[nl_n - 1] != c_min)
			{
				--nl_data[pos];
				for (unsigned i = 0; i < pos; ++i)
					nl_data[i] = c_max - i;
			}
			else
				fill();
		}

		void previous(unsigned times)
		{
			for (unsigned i = 0; i < times; ++i)
				previous();
		}

		bool none() const // Min combination
		{
			data_type value = c_min + nl_n;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				--value;
				if (nl_data[i] != value)
					return false;
			}
			return true;
		}

		bool all() const // Max combination
		{
			data_type value = c_max;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				if (nl_data[i] != value)
					return false;
				--value;
			}
			return true;
		}

		bool any() const // Not min combination
		{
			data_type value = c_min + nl_n;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				--value;
				if (nl_data[i] != value)
					return true;
			}
			return false;
		}

		void fill() // Max combination
		{
			data_type value = c_max;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				nl_data[i] = value;
				--value;
			}
		}

		void reset() // Min combination
		{
			data_type value = c_min + nl_n;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				--value;
				nl_data[i] = value;
			}
		}

		template <typename T>
		T& to(T& num) const
		{
			return num;
		}

		template <typename T>
		void from(const T& num)
		{
			
		}

		data_type min() const
		{
			return c_min;
		}

		data_type max() const
		{
			return c_max;
		}

		void bound(data_type min, data_type max)
		{
			c_min = min;
			c_max = max;
			reset();
		}

		void swap(combination& num) noexcept
		{
			numlist<data_type>::swap(num);
			std::swap(c_min, num.c_min);
			std::swap(c_max, num.c_max);
		}

		combination& operator=(const combination& num)
		{
			numlist<data_type>::operator=(num);
			c_min = num.c_min;
			c_max = num.c_max;
			return *this;
		}

		combination& operator=(combination&& num) noexcept
		{
			numlist<data_type>::operator=(std::move(num));
			c_min = num.c_min;
			c_max = num.c_max;
			return *this;
		}

		combination& operator++()
		{
			next();
			return *this;
		}

		combination operator++(int)
		{
			combination result = *this;
			next();
			return result;
		}

		combination& operator--()
		{
			previous();
			return *this;
		}

		combination operator--(int)
		{
			combination result = *this;
			previous();
			return result;
		}

		combination& operator-=(unsigned times)
		{
			previous(times);
			return *this;
		}

		template <typename T>
		static T count(unsigned n, data_type min, data_type max)
		{
			T pick = 0, result = 0, multiplier = 0, divisor = 0, first_divisor = 0, second_divisor = 0;
			result = 1;
			multiplier = n;
			divisor = 1;
			pick = max - min + 1;
			if (pick > n - pick)
				pick = n - pick;
			for (unsigned i = 0; i < pick; ++i)
			{
				first_divisor = gcd(result, divisor);
				second_divisor = divisor / first_divisor;
				result /= first_divisor;
				result *= multiplier / second_divisor;
				--multiplier;
				++divisor;
			}
			return result;
		}

	private:
		template <typename T>
		static T gcd(T a, T b)
		{
			if (b == 0)
				return a;
			else
				return gcd(b, a % b);
		}

		data_type c_min, c_max;
	};
}
