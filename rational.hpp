#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace zaoly
{
	template <typename d_type, typename s_type>
	class rational // Rational number, an integer divided by a positive integer (always the simplest form, that is, irreducible)
	{
	public:
		rational(d_type d = 0, s_type s = 1) noexcept
		{
			set(d, s);
		}

		void set(d_type d, s_type s = 1) noexcept
		{
			r_dividend = d;
			r_divisor = s;
			reduce();
		}

		void set_div(d_type d, d_type s) noexcept
		{
			if (s >= 0)
			{
				r_dividend = d;
				r_divisor = s;
			}
			else
			{
				r_dividend = -d;
				r_divisor = -s;
			}
			reduce();
		}

		class rational_format_error : public std::exception
		{
		public:
			rational_format_error() : std::exception("Rational number format error") {}
		};

		void set(const char* expression) // Set by string like "3/5", "-11/7", "7/1", "0/1"
		{
			std::stringstream strstrm("");
			d_type d = 0;
			char slash = '\0';
			s_type s = 0;
			strstrm << expression;
			strstrm.exceptions(std::ios::failbit);
			strstrm >> d >> slash;
			if (slash != '/')
				throw rational_format_error();
			strstrm >> s;
			set(d, s);
		}

		void estimate(double num, s_type max_divisor = 1) // Find the closest fraction to a decimal
		{
			d_type d1 = 0, d2 = 0, d = 0, best_d = 0;
			s_type s = 0, best_s = 1;
			double error = 0.0, min_error = 0.0;
			d1 = ceil(num - 0.5);
			d2 = floor(num + 0.5);
			if (d1 == d2 || half_dilemma(d1, d2, 1))
				best_d = d1;
			else
				best_d = d2;
			min_error = abs(num - best_d);
			for (s = 2; s <= max_divisor; ++s)
			{
				d1 = ceil(num * s - 0.5);
				d2 = floor(num * s + 0.5);
				if (d1 == d2 || half_dilemma(d1, d2, s))
					d = d1;
				else
					d = d2;
				error = abs(num - double(d) / double(s));
				if (error < min_error)
				{
					min_error = error;
					best_d = d;
					best_s = s;
				}
			}
			set(best_d, best_s);
		}

		void estimate(const rational& num, s_type max_divisor = 1) // Find the closest fraction to a rational number
		{
			d_type d1 = 0, d2 = 0, d = 0, best_d = 0;
			s_type s = 0, best_s = 1;
			rational error = 0, min_error = 0;
			d1 = ceil(num - rational(1, 2));
			d2 = floor(num + rational(1, 2));
			if (d1 == d2 || half_dilemma(d1, d2, 1))
				best_d = d1;
			else
				best_d = d2;
			min_error = abs(num - best_d);
			for (s = 2; s <= max_divisor; ++s)
			{
				d1 = ceil(num * s - rational(1, 2));
				d2 = floor(num * s + rational(1, 2));
				if (d1 == d2 || half_dilemma(d1, d2, s))
					d = d1;
				else
					d = d2;
				error = abs(num - rational(d, s));
				if (error < min_error)
				{
					min_error = error;
					best_d = d;
					best_s = s;
				}
			}
			set(best_d, best_s);
		}

		d_type dividend() const
		{
			return r_dividend;
		}

		s_type divisor() const
		{
			return r_divisor;
		}

		d_type int_part() const
		{
			return r_dividend / (d_type)r_divisor;
		}

		rational proper_part() const
		{
			return rational(r_dividend % (d_type)r_divisor, r_divisor);
		}

		void print(std::ostream& os = std::cout) const
		{
			os << r_dividend << '/' << r_divisor;
		}

		float to_float() const
		{
			return (float)r_dividend / (float)r_divisor;
		}

		double to_double() const
		{
			return (double)r_dividend / (double)r_divisor;
		}

		char* to_cstr(char* buffer, size_t size) const
		{
			std::stringstream strstrm;
			strstrm << r_dividend << '/' << r_divisor;
			strcpy_s(buffer, size, strstrm.str().c_str());
			return buffer;
		}

		std::string to_str() const
		{
			std::stringstream strstrm;
			strstrm << r_dividend << '/' << r_divisor;
			return strstrm.str();
		}

		rational operator+() const // Positive
		{
			return rational(r_dividend, r_divisor);
		}

		rational operator-() const // Negative
		{
			return rational(-r_dividend, r_divisor);
		}

		rational& operator++()
		{
			r_dividend += r_divisor;
			reduce();
			return *this;
		}

		rational operator++(int)
		{
			rational result = *this;
			r_dividend += r_divisor;
			reduce();
			return result;
		}

		rational& operator--()
		{
			r_dividend -= r_divisor;
			reduce();
			return *this;
		}

		rational operator--(int)
		{
			rational result = *this;
			r_dividend -= r_divisor;
			reduce();
			return result;
		}

		rational& operator+=(const rational& num)
		{
			return *this = *this + num;
		}

		rational& operator-=(const rational& num)
		{
			return *this = *this - num;
		}

		rational& operator*=(const rational& num)
		{
			r_dividend *= num.r_dividend;
			r_divisor *= num.r_divisor;
			reduce();
			return *this;
		}

		rational& operator/=(const rational& num)
		{
			if (num.r_dividend >= 0)
			{
				r_dividend *= num.r_divisor;
				r_divisor *= num.r_dividend;
			}
			else
			{
				r_dividend *= -(d_type)num.r_divisor;
				r_divisor *= -num.r_dividend;
			}
			reduce();
			return *this;
		}

		rational& operator^=(const rational& num)
		{
			return *this = *this ^ num;
		}

	private:
		d_type r_dividend; // Signed
		s_type r_divisor;  // Unsigned (positive)

		s_type gcd(s_type a, s_type b) const // Greatest common divisor (error when b = 0)
		{
			if (a % b == 0)
				return b;
			else
				return gcd(b, a % b);
		}

		void reduce() // Reduce fraction to the simplest form
		{
			s_type factor = gcd(abs(r_dividend), r_divisor);
			r_dividend /= (d_type)factor;
			r_divisor /= factor;
		}

		/* Half dilemma when estimating fraction according to a decimal (or fraction):
		 *
		 * For the decimal 2.5, when the max divisor is 1,
		 * The fractions 2/1 and 3/1, with the same divisor, are equally close to it (both error is 0.5).
		 *
		 * For the decimal 3.75, when the max divisor is 2,
		 * The fractions 7/2 and 4/1 (reduced from 8/2), with the same divisor, are equally close to it (both error is 0.25).
		 *
		 * For the decimal 0.46, when the max divisor is 25,
		 * The fractions 11/25 and 12/25, with the same divisor, are equally close to it (both error is 0.02).
		 *
		 * Rounding off, which often makes the fraction greater, may not be so good here.
		 *
		 * So what is the solution?
		 * In this condition, we arbitrarily pick the one with smaller sum of dividend and divisor (after reducing).
		 *
		 * For the first example, we pick 2/1 (sum 3) instead of 3/1 (sum 4).
		 * For the second example, we pick 4/1 (sum 5) instead of 7/2 (sum 9, maybe this fraction was previously picked).
		 * For the third example, we pick 11/25 (sum 36) instead of 12/25 (sum 37).
		 *
		 * The same is true for estimating according to a fraction.
		 */

		bool half_dilemma(d_type d1, d_type d2, s_type s) const
		{
			rational r1(d1, s), r2(d2, s);
			return abs(r1.r_dividend) + r1.r_divisor <= abs(r2.r_dividend) + r2.r_divisor;
		}
	};

	template <typename d_type, typename s_type>
	d_type fast_pow(d_type base, s_type exponent) // Fast power
	{
		s_type new_exponent = exponent;
		d_type value = base, result = 1;
		while (new_exponent > 0)
		{
			if (new_exponent & 1)
				result *= value;
			new_exponent >>= 1;
			value *= value;
		}
		return result;
	}

	template <typename d_type, typename s_type>
	d_type fast_root(d_type radicand, s_type exponent) // Fast root
	{
		d_type left = 0, right = 0, addend = 0, mid = 0;
		if (exponent <= 0 || radicand < 0 && (exponent & 1) == 0)
			throw exponent;
		left = 1;
		right = 2;
		if (radicand > 0)
		{
			while (fast_pow(right, exponent) <= radicand)
			{
				left <<= 1;
				right <<= 1;
			}
			addend = left;
			mid = left;
			while (addend > 1)
			{
				addend >>= 1;
				mid = left | addend;
				if (fast_pow(mid, exponent) <= radicand)
					left = mid;
			}
			return left;
		}
		else if (radicand < 0)
		{
			while (fast_pow(right, exponent) <= -radicand)
			{
				left <<= 1;
				right <<= 1;
			}
			addend = left;
			mid = left;
			while (addend > 1)
			{
				addend >>= 1;
				mid = left | addend;
				if (fast_pow(mid, exponent) <= -radicand)
					left = mid;
			}
			return -left;
		}
		else
			return 0;
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator+(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b) // Addition: a/b + c/d = (ad+bc)/(bd)
	{
		return rational<d_type, s_type>
		(
			a.dividend() * (d_type)b.divisor() + (d_type)a.divisor() * b.dividend(),
			a.divisor() * b.divisor()
		);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator-(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b) // Subtraction: a/b - c/d = (ad-bc)/(bd)
	{
		return rational<d_type, s_type>
		(
			a.dividend() * (d_type)b.divisor() - (d_type)a.divisor() * b.dividend(),
			a.divisor() * b.divisor()
		);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator*(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b) // Multiplication: a/b * c/d = (ac)/(bd)
	{
		return rational<d_type, s_type>
		(
			a.dividend() * b.dividend(),
			a.divisor() * b.divisor()
		);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator/(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b) // Division: a/b / c/d = (ad)/(bc) (c != 0)
	{
		if (b.dividend() >= 0)
			return rational<d_type, s_type>
			(
				a.dividend() * (d_type)b.divisor(),
				a.divisor() * b.dividend()
			);
		else
			return rational<d_type, s_type>
			(
				-a.dividend() * (d_type)b.divisor(),
				a.divisor() * -b.dividend()
			);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator^(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b) // Power (newly added operator compared to C++ standard)
	{
		if (b.dividend() > 0)
			return rational<d_type, s_type>
			(
				fast_root<d_type, s_type>(fast_pow<d_type, s_type>(a.dividend(), b.dividend()), b.divisor()),
				fast_root<d_type, s_type>(fast_pow<d_type, s_type>(a.divisor(), b.dividend()), b.divisor())
			);
		else if (b.dividend() < 0)
			return rational<d_type, s_type>
			(
				fast_root<d_type, s_type>(fast_pow<d_type, s_type>(a.divisor(), -b.dividend()), b.divisor()),
				fast_root<d_type, s_type>(fast_pow<d_type, s_type>(a.dividend(), -b.dividend()), b.divisor())
			);
		else
			return 1;
	}

	template <typename d_type, typename s_type>
	bool operator==(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b)
	{
		return a.dividend() == b.dividend() && a.divisor() == b.divisor();
	}

	template <typename d_type, typename s_type>
	bool operator!=(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b)
	{
		return a.dividend() != b.dividend() || a.divisor() != b.divisor();
	}

	template <typename d_type, typename s_type>
	bool operator<(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b)
	{
		return a.dividend() * (d_type)b.divisor() < (d_type)a.divisor() * b.dividend();
	}

	template <typename d_type, typename s_type>
	bool operator>(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b)
	{
		return a.dividend() * (d_type)b.divisor() > (d_type)a.divisor() * b.dividend();
	}

	template <typename d_type, typename s_type>
	bool operator<=(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b)
	{
		return a.dividend() * (d_type)b.divisor() <= (d_type)a.divisor() * b.dividend();
	}

	template <typename d_type, typename s_type>
	bool operator>=(const rational<d_type, s_type>& a, const rational<d_type, s_type>& b)
	{
		return a.dividend() * (d_type)b.divisor() >= (d_type)a.divisor() * b.dividend();
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator+(const rational<d_type, s_type>& a, d_type b)
	{
		return a + rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator+(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) + b;
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator-(const rational<d_type, s_type>& a, d_type b)
	{
		return a - rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator-(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) - b;
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator*(const rational<d_type, s_type>& a, d_type b)
	{
		return a * rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator*(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) * b;
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator/(const rational<d_type, s_type>& a, d_type b)
	{
		return a / rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator/(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) / b;
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator^(const rational<d_type, s_type>& a, d_type b)
	{
		return a ^ rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> operator^(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) ^ b;
	}

	template <typename d_type, typename s_type>
	bool operator==(const rational<d_type, s_type>& a, d_type b)
	{
		return a == rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	bool operator==(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) == b;
	}

	template <typename d_type, typename s_type>
	bool operator!=(const rational<d_type, s_type>& a, d_type b)
	{
		return a != rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	bool operator!=(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) != b;
	}

	template <typename d_type, typename s_type>
	bool operator<(const rational<d_type, s_type>& a, d_type b)
	{
		return a < rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	bool operator<(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) < b;
	}

	template <typename d_type, typename s_type>
	bool operator>(const rational<d_type, s_type>& a, d_type b)
	{
		return a > rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	bool operator>(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) > b;
	}

	template <typename d_type, typename s_type>
	bool operator<=(const rational<d_type, s_type>& a, d_type b)
	{
		return a <= rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	bool operator<=(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) <= b;
	}

	template <typename d_type, typename s_type>
	bool operator>=(const rational<d_type, s_type>& a, d_type b)
	{
		return a >= rational<d_type, s_type>(b);
	}

	template <typename d_type, typename s_type>
	bool operator>=(d_type a, const rational<d_type, s_type>& b)
	{
		return rational<d_type, s_type>(a) >= b;
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> abs(const rational<d_type, s_type>& num)
	{
		return rational<d_type, s_type>(std::abs(num.dividend()), num.divisor());
	}

	template <typename d_type, typename s_type>
	d_type floor(const rational<d_type, s_type>& num)
	{
		if (num.dividend() > 0 || num.dividend() % num.divisor() == 0)
			return num.dividend() / num.divisor();
		else
			return num.dividend() / num.divisor() - 1;
	}

	template <typename d_type, typename s_type>
	d_type ceil(const rational<d_type, s_type>& num)
	{
		if (num.dividend() > 0 && num.dividend() % num.divisor() != 0)
			return num.dividend() / num.divisor() + 1;
		else
			return num.dividend() / num.divisor();
	}

	template <typename d_type, typename s_type>
	std::istream& operator>>(std::istream& is, rational<d_type, s_type>& num)
	{
		d_type d = 0;
		char slash = '\0';
		s_type s = 0;
		is >> d;
		if (!is.fail())
		{
			is >> slash;
			if (!is.fail())
				if (slash == '/')
				{
					is >> s;
					if (!is.fail())
						num.set(d, s);
				}
				else
					is.clear(std::ios::failbit);
		}
		return is;
	}

	template <typename d_type, typename s_type>
	std::ostream& operator<<(std::ostream& os, const rational<d_type, s_type>& num)
	{
		num.print(os);
		return os;
	}

	template <typename d_type, typename s_type>
	rational<d_type, s_type> rational_div(d_type d, d_type s)
	{
		rational<d_type, s_type> result;
		result.set_div(d, s);
		return result;
	}
}
