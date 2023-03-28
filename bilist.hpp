#pragma once

#include "../zaoly-numlist/numlist.hpp"

namespace zaoly
{
	class bilist : public numlist<bool>
	{
		using numlist<bool>::nl_n;
		using numlist<bool>::nl_data;
		using numlist<bool>::nl_delimeter;

	public:
		bilist(unsigned n, const char* delimeter = " ") :
			numlist<bool>::numlist(n, delimeter)
		{
			reset();
		}

		void next() // The data at subscript 0 is the lowest
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i])
					nl_data[i] = false;
				else
				{
					nl_data[i] = true;
					return;
				}
		}

		void previous()
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (!nl_data[i])
					nl_data[i] = true;
				else
				{
					nl_data[i] = false;
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
				if (nl_data[i])
					return false;
			return true;
		}

		bool all() const // All max
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (!nl_data[i])
					return false;
			return true;
		}

		bool any() const // Not all min
		{
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i])
					return true;
			return false;
		}

		void extend(unsigned new_size) // When extending, data will not be lost; when shrinking, data will be partially lost
		{
			bool* new_data = nullptr;
			unsigned i = 0;
			new_data = new bool[new_size];
			while (i < nl_n && i < new_size)
			{
				new_data[i] = nl_data[i];
				++i;
			}
			delete[] nl_data;
			nl_data = new_data;
			nl_n = new_size;
			for (; i < nl_n; ++i)
				nl_data[i] = false;
		}

		void set(unsigned index, bool value) // Check boundary and value range
		{
			if (index >= nl_n)
				throw std::out_of_range("Subscript out of range");
			nl_data[index] = value;
		}

		void fill()
		{
			for (unsigned i = 0; i < nl_n; ++i)
				nl_data[i] = true;
		}

		void reset()
		{
			for (unsigned i = 0; i < nl_n; ++i)
				nl_data[i] = false;
		}

		template <typename T>
		T& to(T& num) const
		{
			num = 0;
			if (nl_n > 0)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
				{
					num <<= 1;
					num |= (T)nl_data[i];
				}
				num <<= 1;
				num |= (T)nl_data[0];
			}
			return num;
		}

		template <typename T>
		void from(const T& num)
		{
			T new_num = num;
			for (unsigned i = 0; i < nl_n; ++i)
			{
				nl_data[i] = new_num & 1;
				new_num >>= 1;
				if (new_num == 0)
					return;
			}
		}

		bilist& operator++()
		{
			next();
			return *this;
		}

		bilist operator++(int)
		{
			bilist result = *this;
			next();
			return result;
		}

		bilist& operator--()
		{
			previous();
			return *this;
		}

		bilist operator--(int)
		{
			bilist result = *this;
			previous();
			return result;
		}

		bilist& operator-=(unsigned times)
		{
			previous(times);
			return *this;
		}

		template <typename T>
		static T count(unsigned n)
		{
			return 1 << n;
		}
	};
}
