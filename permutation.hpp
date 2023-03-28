#pragma once

#include "../zaoly-numlist/numlist.hpp"
#include "../zaoly-numlist-noreplace/noreplace.hpp"

namespace zaoly
{
	template <typename data_type>
	class noreplace;

	template <typename data_type>
	class permutation : public numlist<data_type>
	{
		using numlist<data_type>::nl_n;
		using numlist<data_type>::nl_data;
		using numlist<data_type>::nl_delimeter;

	public:
		permutation(unsigned n, data_type min, data_type max, const char* delimeter = " ") :
			numlist<data_type>::numlist(n, delimeter),
			p_min(min),
			p_max(max)
		{
			if (unsigned(max - min + 1) < n)
				throw std::range_error("Number range too narrow");
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
			bool* is_used = new bool[p_max - p_min + 1] {};
			unsigned pos = 0;
			data_type new_result = 0;
			for (unsigned i = 0; i < nl_n; ++i)
				is_used[nl_data[i] - p_min] = true;
			for (pos = 0; pos < nl_n; ++pos)
			{
				is_used[nl_data[pos] - p_min] = false;
				new_result = nl_data[pos];
				while (new_result != p_max)
				{
					++new_result;
					if (!is_used[new_result - p_min])
					{
						nl_data[pos] = new_result;
						is_used[new_result - p_min] = true;
						goto out;
					}
				}
			}
		out:
			new_result = p_min;
			for (unsigned i = pos; i > 0; --i)
			{
				while (is_used[new_result - p_min])
					++new_result;
				is_used[new_result - p_min] = true;
				nl_data[i - 1] = new_result;
			}
			delete[] is_used;
		}

		void previous()
		{
			bool* is_used = new bool[p_max - p_min + 1] {};
			unsigned pos = 0;
			data_type new_result = 0;
			for (unsigned i = 0; i < nl_n; ++i)
				is_used[nl_data[i] - p_min] = true;
			for (pos = 0; pos < nl_n; ++pos)
			{
				is_used[nl_data[pos] - p_min] = false;
				new_result = nl_data[pos];
				while (new_result != p_min)
				{
					--new_result;
					if (!is_used[new_result - p_min])
					{
						nl_data[pos] = new_result;
						is_used[new_result - p_min] = true;
						goto out;
					}
				}
			}
		out:
			new_result = p_max;
			for (unsigned i = pos; i > 0; --i)
			{
				while (is_used[new_result - p_min])
					--new_result;
				is_used[new_result - p_min] = true;
				nl_data[i - 1] = new_result;
			}
			delete[] is_used;
		}

		void previous(unsigned times)
		{
			for (unsigned i = 0; i < times; ++i)
				previous();
		}

		bool none() const // Min permutation
		{
			data_type value = p_min + nl_n;
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
			data_type value = p_max - (nl_n - 1);
			for (unsigned i = 0; i < nl_n; ++i)
			{
				if (nl_data[i] != value)
					return false;
				++value;
			}
			return true;
		}

		bool any() const // Not min permutation
		{
			data_type value = p_min + nl_n;
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
			data_type value = p_max - (nl_n - 1);
			for (unsigned i = 0; i < nl_n; ++i)
			{
				nl_data[i] = value;
				++value;
			}
		}

		void reset() // Min permutation
		{
			data_type value = p_min + nl_n;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				--value;
				nl_data[i] = value;
			}
		}

		template <typename T>
		T& to(T& num) const
		{
			noreplace<data_type> nr(nl_n, p_min, p_max, nl_delimeter);
			nr.from_permutation(*this);
			nr.to(num);
			return num;
		}

		template <typename T>
		void from(const T& num)
		{
			noreplace<data_type> nr(nl_n, p_min, p_max, nl_delimeter);
			nr.from(num);
			from_noreplace(nr);
		}

		data_type min() const
		{
			return p_min;
		}

		data_type max() const
		{
			return p_max;
		}

		void bound(data_type min, data_type max)
		{
			p_min = min;
			p_max = max;
			reset();
		}

		void from_noreplace(const noreplace<data_type>& nr)
		{
			bool* is_used = new bool[nr.max() - nr.min() + 1] {};
			numlist<data_type>::resize(nr.size());
			nl_delimeter = nr.delimeter();
			bound(nr.min(), nr.max());
			for (unsigned i = nl_n - 1; i > 0; --i)
			{
				nl_data[i] = p_min;
				while (is_used[nl_data[i] - p_min])
					++nl_data[i];
				for (unsigned j = 0; j < nr[i] - p_min; ++j)
					do
						++nl_data[i];
					while (is_used[nl_data[i] - p_min]);
				is_used[nl_data[i] - p_min] = true;
			}
			nl_data[0] = p_min;
			while (is_used[nl_data[0] - p_min])
				++nl_data[0];
			for (unsigned j = 0; j < nr[0] - p_min; ++j)
				do
					++nl_data[0];
				while (is_used[nl_data[0] - p_min]);
		}

		void swap(permutation& num) noexcept
		{
			numlist<data_type>::swap(num);
			std::swap(p_min, num.p_min);
			std::swap(p_max, num.p_max);
		}

		permutation& operator=(const permutation& num)
		{
			numlist<data_type>::operator=(num);
			p_min = num.p_min;
			p_max = num.p_max;
			return *this;
		}

		permutation& operator=(permutation&& num) noexcept
		{
			numlist<data_type>::operator=(std::move(num));
			p_min = num.p_min;
			p_max = num.p_max;
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
		data_type p_min, p_max;
	};
}
