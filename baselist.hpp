#pragma once

#include "../zaoly-numlist/numlist.hpp"

namespace zaoly
{
	template <typename data_type>
	class baselist : public numlist<data_type>
	{
		using numlist<data_type>::nl_n;
		using numlist<data_type>::nl_data;
		using numlist<data_type>::nl_delimeter;

	public:
		baselist(unsigned n, data_type min, data_type max, const char* delimeter = " ") :
			numlist<data_type>::numlist(n, delimeter),
			bl_min(min),
			bl_max(max)
		{
			reset();
		}

		baselist(const baselist& num) : numlist<data_type>::numlist(num.nl_n, num.nl_delimeter)
		{
			*this = num;
		}

		baselist(baselist&& num) noexcept : numlist<data_type>::numlist(num.nl_n, num.nl_delimeter)
		{
			*this = std::move(num);
		}

		void next() // The data at subscript 0 is the lowest
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] == bl_max)
					nl_data[i] = bl_min;
				else
				{
					++nl_data[i];
					return;
				}
		}

		void previous()
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] == bl_min)
					nl_data[i] = bl_max;
				else
				{
					--nl_data[i];
					return;
				}
		}

		void previous(unsigned times)
		{
			for (unsigned i = 0; i < times; ++i)
				previous();
		}

		bool none() const // All min
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] != bl_min)
					return false;
			return true;
		}

		bool all() const // All max
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] != bl_max)
					return false;
			return true;
		}

		bool any() const // Not all min
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] != bl_min)
					return true;
			return false;
		}

		void extend(unsigned new_size) // When extending, data will not be lost; when shrinking, data will be partially lost
		{
			data_type* new_data = nullptr;
			unsigned i = 0;
			new_data = new data_type[new_size];
			while (i < nl_n && i < new_size)
			{
				new_data[i] = nl_data[i];
				++i;
			}
			delete[] nl_data;
			nl_data = new_data;
			nl_n = new_size;
			for (; i < nl_n; ++i)
				nl_data[i] = bl_min;
		}

		void set(unsigned index, data_type value) // Check boundary and value range; for data, only operator "<" is used
		{
			if (index >= nl_n)
				throw std::out_of_range("Subscript out of range");
			if (value < bl_min || !(bl_max < value))
				throw std::out_of_range("Data out of range");
			nl_data[index] = value;
		}

		void fill()
		{
			for (unsigned i = 0; i < nl_n; ++i)
				nl_data[i] = bl_max;
		}

		void reset()
		{
			for (unsigned i = 0; i < nl_n; ++i)
				nl_data[i] = bl_min;
		}

		template <typename T>
		T& to(T& num) const
		{
			num = 0;
			if (nl_n > 0)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
				{
					num *= bl_max - bl_min + 1;
					num += nl_data[i] - bl_min;
				}
				num *= bl_max - bl_min + 1;
				num += nl_data[0] - bl_min;
			}
			return num;
		}

		template <typename T>
		void from(const T& num)
		{
			T new_num = num;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				nl_data[i] = new_num % (bl_max - bl_min + 1) + bl_min;
				new_num /= bl_max - bl_min + 1;
				if (new_num == 0)
					return;
			}
		}

		data_type min() const
		{
			return bl_min;
		}

		data_type max() const
		{
			return bl_max;
		}

		void bound(data_type min, data_type max)
		{
			bl_min = min;
			bl_max = max;
			reset();
		}

		void swap(baselist& num) noexcept
		{
			numlist<data_type>::swap(num);
			std::swap(bl_min, num.bl_min);
			std::swap(bl_max, num.bl_max);
		}

		baselist& operator=(const baselist& num)
		{
			numlist<data_type>::operator=(num);
			bl_min = num.bl_min;
			bl_max = num.bl_max;
			return *this;
		}

		baselist& operator=(baselist&& num) noexcept
		{
			numlist<data_type>::operator=(std::move(num));
			bl_min = num.bl_min;
			bl_max = num.bl_max;
			return *this;
		}

		baselist& operator++()
		{
			next();
			return *this;
		}

		baselist operator++(int)
		{
			baselist result = *this;
			next();
			return result;
		}

		baselist& operator--()
		{
			previous();
			return *this;
		}

		baselist operator--(int)
		{
			baselist result = *this;
			previous();
			return result;
		}

		baselist& operator-=(unsigned times)
		{
			previous(times);
			return *this;
		}

		template <typename T>
		static T count(unsigned n, data_type min, data_type max)
		{
			unsigned i = 0;
			T result = 0;
			result = 1;
			for (i = 0; i < n; ++i)
				result *= max - min + 1;
			return result;
		}

	private:
		data_type bl_min, bl_max;
	};
}
