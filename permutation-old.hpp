#pragma once

#include "../zaoly-numlist/numlist.hpp"

namespace zaoly
{
	template <typename data_type>
	class permutation : public numlist<data_type>
	{
		using numlist<data_type>::nl_n;
		using numlist<data_type>::nl_data;
		using numlist<data_type>::nl_delimeter;

	public:
		permutation(unsigned n, data_type min = 0, const char* delimeter = " ") :
			numlist<data_type>::numlist(n, delimeter),
			p_min(min)
		{
			reset();
		}

		permutation(const permutation& num) : numlist<data_type>::numlist(num.nl_n, num.nl_delimeter)
		{
			*this = num;
		}

		permutation(permutation&& num) noexcept : numlist<data_type>::numlist(num.nl_n, num.nl_delimeter)
		{
			*this = std::move(num);
		}

		void next() // The data at subscript 0 is the easiest to change. Note that the permutation is stored in reverse order!
		{
			for (unsigned i = 0; i < nl_n - 1; ++i)
				if (nl_data[i] > nl_data[i + 1])
				{
					unsigned j = 0;
					for (; j <= i; ++j)
						if (nl_data[j] > nl_data[i + 1])
							break;
					std::swap(nl_data[j], nl_data[i + 1]);
					reverse_data(0, i);
					return;
				}
			reverse_data(0, nl_n - 1);
		}

		void previous()
		{
			
		}

		void previous(unsigned times)
		{
			for (unsigned i = 0; i < times; ++i)
				previous();
		}

		bool none() const // Min permutation
		{
			unsigned value = nl_n + p_min;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				--value;
				if (nl_data[i] != value)
					return false;
			}
			return true;
		}

		bool all() const // Max permutation
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] != i + p_min)
					return false;
			return true;
		}

		bool any() const // Not min permutation
		{
			unsigned value = nl_n + p_min;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				--value;
				if (nl_data[i] != value)
					return true;
			}
			return false;
		}

		void fill() // Max permutation
		{
			for (unsigned i = 0; i < nl_n; ++i)
				nl_data[i] = i + p_min;
		}

		void reset() // Min permutation
		{
			unsigned value = nl_n + p_min;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				--value;
				nl_data[i] = value;
			}
		}

		template <typename T>
		T& to(T& num) const
		{
			T result = 0, multiplier = 0;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				
			}
		}

		template <typename T>
		void from(const T& num)
		{
			//
		}

		permutation& operator=(const permutation& num)
		{
			numlist<data_type>::operator=(num);
			p_min = num.p_min;
			return *this;
		}

		permutation& operator=(permutation&& num) noexcept
		{
			numlist<data_type>::operator=(std::move(num));
			p_min = num.p_min;
			return *this;
		}

		permutation& operator++()
		{
			next();
			return *this;
		}

		permutation operator++(int)
		{
			permutation result = *this;
			next();
			return result;
		}

		permutation& operator--()
		{
			previous();
			return *this;
		}

		permutation operator--(int)
		{
			permutation result = *this;
			previous();
			return result;
		}

		permutation& operator-=(unsigned times)
		{
			previous(times);
			return *this;
		}
		
	private:
		void reverse_data(unsigned begin, unsigned end)
		{
			unsigned left = begin, right = end;
			while (right > left)
			{
				std::swap(nl_data[left], nl_data[right]);
				++left;
				--right;
			}
		}

		data_type p_min;
	};
}
