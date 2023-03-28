#pragma once

#include <iostream>

namespace zaoly
{
	template <typename data_type>
	class numlist
	{
	public:
		numlist(unsigned n, const char* delimeter = " ") :
			nl_n(n),
			nl_delimeter(delimeter)
		{
			if (nl_n > 0)
				nl_data = new data_type[nl_n];
			else
				nl_data = nullptr;
		}

		numlist(const numlist& num)
		{
			*this = num;
		}

		numlist(numlist&& num) noexcept
		{
			*this = std::move(num);
		}

		~numlist()
		{
			delete[] nl_data;
		}

		virtual void next() = 0;

		void next(unsigned times)
		{
			for (unsigned i = 0; i < times; ++i)
				next();
		}

		virtual bool none() const = 0;
		virtual bool all() const = 0;
		virtual bool any() const = 0;

		unsigned size() const
		{
			return nl_n;
		}

		void resize(unsigned new_size) // Clear data
		{
			nl_n = new_size;
			delete[] nl_data;
			if (nl_n > 0)
			{
				nl_data = new data_type[nl_n];
				reset();
			}
			else
				nl_data = nullptr;
		}

		const data_type& get(unsigned index) const // Check boundary
		{
			if (index >= nl_n)
				throw std::out_of_range("Subscript out of range");
			return nl_data[index];
		}

		virtual void reset() = 0;
		
		const char* delimeter() const
		{
			return nl_delimeter;
		}

		void delimeter(const char* str)
		{
			nl_delimeter = str;
		}

		void print(std::ostream& os = std::cout) const // In reverse order
		{
			if (nl_n > 0)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					os << nl_data[i] << nl_delimeter;
				os << nl_data[0];
			}
		}

		virtual void swap(numlist& num) noexcept
		{
			std::swap(nl_n, num.nl_n);
			std::swap(nl_data, num.nl_data);
			std::swap(nl_delimeter, num.nl_delimeter);
		}

		virtual numlist& operator=(const numlist& num)
		{
			data_type* new_data = nullptr;
			nl_n = num.nl_n;
			nl_delimeter = num.nl_delimeter;
			if (nl_n > 0)
			{
				new_data = new data_type[nl_n];
				for (unsigned i = 0; i < nl_n; ++i)
					new_data[i] = num.nl_data[i];
			}
			else
				new_data = nullptr;
			delete[] nl_data;
			nl_data = new_data;
			return *this;
		}

		virtual numlist& operator=(numlist&& num) noexcept
		{
			nl_n = num.nl_n;
			nl_delimeter = num.nl_delimeter;
			std::swap(nl_data, num.nl_data);
			return *this;
		}

		numlist& operator++()
		{
			next();
			return *this;
		}

		numlist operator++(int)
		{
			numlist result = *this;
			next();
			return result;
		}

		numlist& operator+=(unsigned times)
		{
			next(times);
			return *this;
		}

		bool operator==(const numlist& num) const
		{
			if (nl_n != num.nl_n)
				return false;
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] != num.nl_data[i])
					return false;
			return true;
		}

		bool operator!=(const numlist& num) const
		{
			if (nl_n != num.nl_n)
				return true;
			for (unsigned i = 0; i < nl_n; ++i)
				if (nl_data[i] != num.nl_data[i])
					return true;
			return false;
		}

		bool operator<(const numlist& num) const // Lexicographically
		{
			if (nl_n < num.nl_n)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					if (nl_data[i] < num.nl_data[i])
						return true;
					else if (nl_data[i] > num.nl_data[i])
						return false;
				if (nl_data[0] < num.nl_data[0])
					return true;
				else if (nl_data[0] > num.nl_data[0])
					return false;
				return true;
			}
			else if (nl_n == num.nl_n)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					if (nl_data[i] < num.nl_data[i])
						return true;
					else if (nl_data[i] > num.nl_data[i])
						return false;
				if (nl_data[0] < num.nl_data[0])
					return true;
				else if (nl_data[0] > num.nl_data[0])
					return false;
				return false;
			}
			else if (nl_n > num.nl_n)
			{
				for (unsigned i = 0; i < num.nl_n; ++i)
					if (nl_data[i] < num.nl_data[i])
						return true;
					else if (nl_data[i] > num.nl_data[i])
						return false;
				if (nl_data[0] < num.nl_data[0])
					return true;
				else if (nl_data[0] > num.nl_data[0])
					return false;
				return false;
			}
		}

		bool operator>(const numlist& num) const
		{
			if (nl_n < num.nl_n)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					if (nl_data[i] > num.nl_data[i])
						return true;
					else if (nl_data[i] < num.nl_data[i])
						return false;
				if (nl_data[0] > num.nl_data[0])
					return true;
				else if (nl_data[0] < num.nl_data[0])
					return false;
				return false;
			}
			else if (nl_n == num.nl_n)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					if (nl_data[i] > num.nl_data[i])
						return true;
					else if (nl_data[i] < num.nl_data[i])
						return false;
				if (nl_data[0] > num.nl_data[0])
					return true;
				else if (nl_data[0] < num.nl_data[0])
					return false;
				return false;
			}
			else if (nl_n > num.nl_n)
			{
				for (unsigned i = 0; i < num.nl_n; ++i)
					if (nl_data[i] > num.nl_data[i])
						return true;
					else if (nl_data[i] < num.nl_data[i])
						return false;
				if (nl_data[0] > num.nl_data[0])
					return true;
				else if (nl_data[0] < num.nl_data[0])
					return false;
				return true;
			}
		}

		bool operator<=(const numlist& num) const
		{
			if (nl_n < num.nl_n)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					if (nl_data[i] < num.nl_data[i])
						return true;
					else if (nl_data[i] > num.nl_data[i])
						return false;
				if (nl_data[0] < num.nl_data[0])
					return true;
				else if (nl_data[0] > num.nl_data[0])
					return false;
				return true;
			}
			else if (nl_n == num.nl_n)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					if (nl_data[i] < num.nl_data[i])
						return true;
					else if (nl_data[i] > num.nl_data[i])
						return false;
				if (nl_data[0] < num.nl_data[0])
					return true;
				else if (nl_data[0] > num.nl_data[0])
					return false;
				return true;
			}
			else if (nl_n > num.nl_n)
			{
				for (unsigned i = 0; i < num.nl_n; ++i)
					if (nl_data[i] < num.nl_data[i])
						return true;
					else if (nl_data[i] > num.nl_data[i])
						return false;
				if (nl_data[0] < num.nl_data[0])
					return true;
				else if (nl_data[0] > num.nl_data[0])
					return false;
				return false;
			}
		}

		bool operator>=(const numlist& num) const
		{
			if (nl_n < num.nl_n)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					if (nl_data[i] > num.nl_data[i])
						return true;
					else if (nl_data[i] < num.nl_data[i])
						return false;
				if (nl_data[0] > num.nl_data[0])
					return true;
				else if (nl_data[0] < num.nl_data[0])
					return false;
				return false;
			}
			else if (nl_n == num.nl_n)
			{
				for (unsigned i = nl_n - 1; i > 0; --i)
					if (nl_data[i] > num.nl_data[i])
						return true;
					else if (nl_data[i] < num.nl_data[i])
						return false;
				if (nl_data[0] > num.nl_data[0])
					return true;
				else if (nl_data[0] < num.nl_data[0])
					return false;
				return true;
			}
			else if (nl_n > num.nl_n)
			{
				for (unsigned i = 0; i < num.nl_n; ++i)
					if (nl_data[i] > num.nl_data[i])
						return true;
					else if (nl_data[i] < num.nl_data[i])
						return false;
				if (nl_data[0] > num.nl_data[0])
					return true;
				else if (nl_data[0] < num.nl_data[0])
					return false;
				return true;
			}
		}

		const data_type& operator[](unsigned index) const
		{
			return get(index);
		}

	protected:
		unsigned nl_n;
		data_type* nl_data;
		const char* nl_delimeter;
	};

	template <typename data_type>
	std::ostream& operator<<(std::ostream& os, const numlist<data_type>& num)
	{
		num.print(os);
		return os;
	}
}
