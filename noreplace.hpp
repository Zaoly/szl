#pragma once

#include "../zaoly-numlist/numlist.hpp"
#include "../zaoly-numlist-permutation/permutation.hpp"

namespace zaoly
{
	template <typename data_type>
	class permutation;

	template <typename data_type>
	class noreplace : public numlist<data_type>
	{
		using numlist<data_type>::nl_n;
		using numlist<data_type>::nl_data;
		using numlist<data_type>::nl_delimeter;

	public:
		noreplace(unsigned n, data_type min, data_type max, const char* delimeter = " ") :
			numlist<data_type>::numlist(n, delimeter),
			nr_min(min),
			nr_max(max)
		{
			if (unsigned(max - min + 1) < n)
				throw std::range_error("Number range too narrow");
			reset();
		}

		noreplace(const noreplace& num) : numlist<data_type>::numlist(num.nl_n, num.nl_delimeter)
		{
			*this = num;
		}

		noreplace(noreplace&& num) noexcept : numlist<data_type>::numlist(num.nl_n, num.nl_delimeter)
		{
			*this = std::move(num);
		}

		void next() // The data at subscript 0 is the easiest to change. Note that the no-replace list is stored in reverse order!
		{
			data_type value = nr_max - (nl_n - 1);
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] == value)
				{
					nl_data[i] = nr_min;
					++value;
				}
				else
				{
					++nl_data[i];
					break;
				}
		}

		void previous()
		{
			data_type value = nr_max - (nl_n - 1);
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] == nr_min)
				{
					nl_data[i] = value;
					++value;
				}
				else
				{
					--nl_data[i];
					break;
				}
		}

		void previous(unsigned times)
		{
			for (unsigned i = 0; i < times; ++i)
				previous();
		}

		bool none() const // Min no-replace list
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] != nr_min)
					return false;
			return true;
		}

		bool all() const // Max no-replace list
		{
			data_type value = nr_max - (nl_n - 1);
			for (unsigned i = 0; i < nl_n; ++i)
			{
				if (nl_data[i] != value)
					return false;
				++value;
			}
			return true;
		}

		bool any() const // Not min no-replace list
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] != nr_min)
					return true;
			return false;
		}

		void fill() // Max no-replace list
		{
			data_type value = nr_max - (nl_n - 1);
			for (unsigned i = 0; i < nl_n; ++i)
			{
				nl_data[i] = value;
				++value;
			}
		}

		void reset() // Min no-replace list
		{
			for (unsigned i = 0; i < nl_n; ++i)
				nl_data[i] = nr_min;
		}

		template <typename T>
		T& to(T& num) const
		{
			T multiplier = nr_max - nr_min;
			num = 0;
			for (unsigned i = nl_n - 1; i > 0; --i)
			{
				num += nl_data[i] - nr_min;
				num *= multiplier;
				--multiplier;
			}
			num += nl_data[0] - nr_min;
			return num;
		}

		template <typename T>
		void from(const T& num)
		{
			T new_num = num, divisor = nr_max - nr_min - (nl_n - 1);
			for (unsigned i = 0; i < nl_n; ++i)
			{
				++divisor;
				nl_data[i] = new_num % divisor + nr_min;
				new_num /= divisor;
				if (new_num == 0)
					return;
			}
		}

		data_type min() const
		{
			return nr_min;
		}

		data_type max() const
		{
			return nr_max;
		}

		void bound(data_type min, data_type max)
		{
			nr_min = min;
			nr_max = max;
			reset();
		}

		void from_permutation(const permutation<data_type>& p)
		{
			unsigned j = 0;
			numlist<data_type>::resize(p.size());
			nl_delimeter = p.delimeter();
			bound(p.min(), p.max());
			for (unsigned i = nl_n - 1; i > 0; --i)
			{
				nl_data[i] = p[i];
				for (j = nl_n - 1; j > i; --j)
					if (p[j] < p[i])
						--nl_data[i];
			}
			nl_data[0] = p[0];
			for (j = nl_n - 1; j > 0; --j)
				if (p[j] < p[0])
					--nl_data[0];
		}

		void swap(noreplace& num) noexcept
		{
			numlist<data_type>::swap(num);
			std::swap(nr_min, num.nr_min);
			std::swap(nr_max, num.nr_max);
		}

		noreplace& operator=(const noreplace& num)
		{
			numlist<data_type>::operator=(num);
			nr_min = num.nr_min;
			nr_max = num.nr_max;
			return *this;
		}

		noreplace& operator=(noreplace&& num) noexcept
		{
			numlist<data_type>::operator=(std::move(num));
			nr_min = num.nr_min;
			nr_max = num.nr_max;
			return *this;
		}

		noreplace& operator++()
		{
			next();
			return *this;
		}

		noreplace operator++(int)
		{
			noreplace result = *this;
			next();
			return result;
		}

		noreplace& operator--()
		{
			previous();
			return *this;
		}

		noreplace operator--(int)
		{
			noreplace result = *this;
			previous();
			return result;
		}

		noreplace& operator-=(unsigned times)
		{
			previous(times);
			return *this;
		}

		template <typename T>
		static T count(unsigned n, data_type min, data_type max)
		{
			unsigned i = 0;
			T multiplier = 0, result = 0;
			result = 1;
			multiplier = max - min + 1;
			for (; i < n; ++i)
			{
				result *= multiplier;
				--multiplier;
			}
			return result;
		}

	private:
		data_type nr_min, nr_max;
	};
}
