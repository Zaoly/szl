#pragma once

#include <iostream>

namespace zaoly
{
	// The following is about operating sign

	using uchar = unsigned char;

	enum class oper_sign : uchar { add = 0U, subtr = 1U, mult = 2U, div = 3U };

	oper_sign& operator++(oper_sign& ops) noexcept
	{
		if (ops == oper_sign::div)
			return ops = oper_sign::add; // Wrap around
		else
			return ops = oper_sign((unsigned)ops + 1);
	}

	oper_sign& operator--(oper_sign& ops) noexcept
	{
		if (ops == oper_sign::add)
			return ops = oper_sign::div;
		else
			return ops = oper_sign((unsigned)ops - 1);
	}

	oper_sign operator++(oper_sign& ops, int) noexcept
	{
		oper_sign result = ops;
		if (ops == oper_sign::div)
			ops = oper_sign::add;
		else
			ops = oper_sign((unsigned)ops + 1);
		return result;
	}

	oper_sign operator--(oper_sign& ops, int) noexcept
	{
		oper_sign result = ops;
		if (ops == oper_sign::add)
			ops = oper_sign::div;
		else
			ops = oper_sign((unsigned)ops - 1);
		return result;
	}

	const char* oper_sign_to_str(oper_sign s) noexcept
	{
		switch (s)
		{
		case oper_sign::add:
			return "add";
		case oper_sign::subtr:
			return "subtract";
		case oper_sign::mult:
			return "multiply";
		case oper_sign::div:
			return "divide";
		default:
			return nullptr;
		}
	}

	class oper_sign_error : public std::invalid_argument
	{
	public:
		oper_sign_error() noexcept : std::invalid_argument("Operating sign error") {}
	};

	char oper_sign_to_char(oper_sign s)
	{
		switch (s)
		{
		case oper_sign::add:
			return '+';
		case oper_sign::subtr:
			return '-';
		case oper_sign::mult:
			return '*';
		case oper_sign::div:
			return '/';
		default:
			throw oper_sign_error();
		}
	}

	class divide_by_zero_error : public std::logic_error
	{
	public:
		divide_by_zero_error() noexcept : std::logic_error("Divide by zero error") {}
	};

	template <typename number>
	number calc_between(number a, number b, oper_sign s)
	{
		switch (s)
		{
		case oper_sign::add:
			return a + b; // There is no need for "break" after "return"
		case oper_sign::subtr:
			return a - b;
		case oper_sign::mult:
			return a * b;
		case oper_sign::div:
			if (b == 0)
				throw divide_by_zero_error();
			return a / b;
		default:
			throw oper_sign_error();
		}
	}

	// The following is about no-repeat pair

	class equal_pair_error : public std::logic_error
	{
	public:
		equal_pair_error() noexcept : std::logic_error("Equal pair error") {}
	};

	class norepeat_pair // Two numbers which are not equal and less than a number (N). Note that this structure does not store the number N
	{
	public:
		norepeat_pair(unsigned val = 0U) noexcept : p_val(val) {}

		norepeat_pair(unsigned a, unsigned b, unsigned n)
		{
			pack(a, b, n);
		}

		unsigned a(unsigned n) const
		{
			if (p_val >= n * (n - 1))
				throw std::out_of_range("Pair value out of range");
			return p_val / (n - 1);
		}

		unsigned b(unsigned n) const
		{
			unsigned temp_result = 0;
			if (p_val >= n * (n - 1))
				throw std::out_of_range("Pair value out of range");
			temp_result = p_val % (n - 1);
			if (temp_result < p_val / (n - 1))
				return temp_result;
			else
				return temp_result + 1;
		}

		void pack(unsigned a, unsigned b, unsigned n)
		{
			if (a >= n || b >= n)
				throw std::out_of_range("Pair element out of range");
			else if (b < a)
				p_val = a * (n - 1) + b;
			else if (b > a)
				p_val = a * (n - 1) + (b - 1);
			else
				throw equal_pair_error();
		}

		void next(unsigned n) noexcept
		{
			if (p_val >= n * (n - 1) - 1)
				p_val = 0; // Wrap around
			else
				++p_val;
		}

		void previous(unsigned n) noexcept
		{
			if (p_val == 0)
				p_val = n * (n - 1) - 1;
			else
				--p_val;
		}

		operator unsigned() const noexcept
		{
			return p_val;
		}

	private:
		unsigned p_val;
	};

	// The following is about formula
	
	template <typename number>
	class formula
	{
	public:
		formula(unsigned n) : f_n(n)
		{
			if (f_n <= 1)
				throw std::out_of_range("N is too small");
			f_nums = new number[f_n]{};
			f_order_pairs = new norepeat_pair[f_n - 1]{};
			f_signs = new oper_sign[f_n - 1]{};
		}

		formula(const formula& f) noexcept
		{
			copy_assign(f);
		}

		formula(formula&& f) noexcept
		{
			move_assign(std::move(f));
		}

		~formula()
		{
			delete[] f_nums;
			delete[] f_order_pairs;
			delete[] f_signs;
		}

		unsigned n() const noexcept // Number of numbers to calculate
		{
			return f_n;
		}

		number* nums() const noexcept
		{
			return f_nums;
		}

		number& num(unsigned index) const
		{
			if (index >= f_n)
				throw std::out_of_range("Subscript out of range");
			return f_nums[index];
		}

		void nums(const number* from) noexcept
		{
			for (unsigned i = 0; i < f_n; ++i)
				f_nums[i] = from[i];
		}

		norepeat_pair* order_pairs() const noexcept
		{
			return f_order_pairs;
		}

		norepeat_pair& order_pair(unsigned index) const
		{
			if (index >= f_n - 1)
				throw std::out_of_range("Subscript out of range");
			return f_order_pairs[index];
		}

		void order_pairs(const norepeat_pair* from) noexcept
		{
			for (unsigned i = 0; i < f_n - 1; ++i)
				f_order_pairs[i] = from[i];
		}

		void next_order_pairs() noexcept
		{
			for (unsigned i = 0; i < f_n - 1; ++i)
			{
				f_order_pairs[i].next(f_n - i);
				if (f_order_pairs[i] != 0)
					return;
			}
		}

		bool order_pairs_any() const noexcept
		{
			for (unsigned i = 0; i < f_n - 1; ++i)
				if (f_order_pairs[i] != 0)
					return true;
			return false;
		}

		void reset_order_pairs() noexcept
		{
			for (unsigned i = 0; i < f_n - 1; ++i)
				f_order_pairs[i] = 0;
		}

		oper_sign* signs() const noexcept
		{
			return f_signs;
		}

		oper_sign& sign(unsigned index) const
		{
			if (index >= f_n - 1)
				throw std::out_of_range("Subscript out of range");
			return f_signs[index];
		}

		void signs(const oper_sign* from) noexcept
		{
			for (unsigned i = 0; i < f_n - 1; ++i)
				f_signs[i] = from[i];
		}

		void next_signs() noexcept
		{
			for (unsigned i = 0; i < f_n - 1; ++i)
			{
				++f_signs[i];
				if (f_signs[i] != oper_sign::add)
					return;
			}
		}

		bool signs_any() const noexcept
		{
			for (unsigned i = 0; i < f_n - 1; ++i)
				if (f_signs[i] != oper_sign::add)
					return true;
			return false;
		}

		void reset_signs() noexcept
		{
			for (unsigned i = 0; i < f_n - 1; ++i)
				f_signs[i] = oper_sign::add;
		}

		number calculate
		(
			bool print_or_not = false,
			const char* delimeter1 = " ",
			const char* delimeter2 = "\n",
			std::ostream& os = std::cout
		) const
		{
			number* intermediate = nullptr, result = 0;
			intermediate = new number[f_n];
			for (unsigned index = 0; index < f_n; ++index)
				intermediate[index] = f_nums[index];
			for (unsigned step = 0; step < f_n - 1; ++step)
			{
				unsigned a = 0, b = 0, index = 0;
				a = f_order_pairs[step].a(f_n - step);
				b = f_order_pairs[step].b(f_n - step);
				if (print_or_not)
				{
					if (step > 0)
						os << delimeter2;
					os
						<< intermediate[a] << delimeter1
						<< oper_sign_to_char(f_signs[step]) << delimeter1
						<< intermediate[b] << delimeter1
						<< '=' << delimeter1;
				}
				if (a < b)
				{
					while (index < a)
						++index;
					intermediate[a] = calc_between(intermediate[a], intermediate[b], f_signs[step]);
					if (print_or_not)
						os << intermediate[a];
					while (index < b)
						++index;
				}
				else if (a > b)
				{
					while (index < b)
						++index;
					intermediate[b] = calc_between(intermediate[a], intermediate[b], f_signs[step]);
					if (print_or_not)
						os << intermediate[b];
					while (index < a)
						++index;
				}
				while (index < f_n - step - 1)
				{
					intermediate[index] = intermediate[index + 1];
					++index;
				}
			}
			result = intermediate[0];
			delete[] intermediate;
			return result;
		}

		void swap(formula& f) noexcept
		{
			std::swap(f_n, f.f_n);
			std::swap(f_order_pairs, f.f_order_pairs);
			std::swap(f_signs, f.f_signs);
		}

	private:
		unsigned f_n;
		number* f_nums;
		norepeat_pair* f_order_pairs;
		oper_sign* f_signs;

		void copy_assign(const formula& f) noexcept
		{
			f_n = f.f_n;
			delete[] f_nums;
			delete[] f_order_pairs;
			delete[] f_signs;
			f_nums = new number[f_n];
			f_order_pairs = new norepeat_pair[f_n - 1];
			f_signs = new oper_sign[f_n - 1];
			for (unsigned i = 0; i < f_n; ++i)
				f_nums[i] = f.f_nums[i];
			for (unsigned i = 0; i < f_n - 1; ++i)
			{
				f_order_pairs[i] = f.f_order_pairs[i];
				f_signs[i] = f.f_signs[i];
			}
		}

		void move_assign(formula&& f) noexcept
		{
			f_n = f.f_n;
			std::swap(f_nums, f.f_nums);
			std::swap(f_order_pairs, f.f_order_pairs);
			std::swap(f_signs, f.f_signs);
		}
	};
}
