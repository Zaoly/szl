#pragma once

#include "../zaoly-exact-trig-functions/exact-trig.hpp"
#include <cmath>
#include <map>
#include <stdexcept>
#include <vector>

namespace zaoly
{
	class end_no_greater_than_begin : public std::logic_error
	{
	public:
		end_no_greater_than_begin() : std::logic_error("End value is no greater than begin value") {}
	};

	class end_less_than_begin : public std::logic_error
	{
	public:
		end_less_than_begin() : std::logic_error("End value is less than begin value") {}
	};

	template <typename index_type, typename value_type>
	class interpolator
	{
	public:
		interpolator() {}

		interpolator(const std::map<index_type, value_type>& table)
		{
			set(table);
		}

		virtual void add(index_type index, value_type value)
		{
			index_values[index] = value;
		}

		virtual void set(const std::map<index_type, value_type>& table)
		{
			index_values = table;
		}

		virtual void clear()
		{
			index_values.clear();
		}
		
		virtual value_type get(index_type index) const = 0;

	protected:
		std::map<index_type, value_type> index_values;
	};

	template <typename index_type, typename value_type>
	class constant_interpolator : public interpolator<index_type, value_type>
	{
		using interpolator<index_type, value_type>::index_values;

	public:
		using interpolator<index_type, value_type>::set;

		constant_interpolator(value_type leftmost = {}) : i_leftmost(leftmost) {}

		constant_interpolator(const std::map<index_type, value_type>& table, value_type leftmost = {}) :
			i_leftmost(leftmost)
		{
			set(table);
		}

		void leftmost(value_type value)
		{
			i_leftmost = value;
		}

		value_type get(index_type index) const override
		{
			using const_iterator = typename std::map<index_type, value_type>::const_iterator;
			const_iterator iter = index_values.upper_bound(index);
			if (iter == index_values.begin())
				return i_leftmost;
			else
			{
				--iter;
				return iter->second;
			}
		}

		template <typename fun_type>
		value_type best(index_type begin, index_type end, fun_type predicative) const
		{
			using const_iterator = typename std::map<index_type, value_type>::const_iterator;
			const_iterator begin_iter, end_iter, iter;
			value_type result{};
			if (end <= begin)
				throw end_no_greater_than_begin();
			begin_iter = index_values.upper_bound(begin);
			if (begin_iter == index_values.begin())
				result = i_leftmost;
			else
			{
				--begin_iter;
				result = begin_iter->second;
				++begin_iter;
			}
			end_iter = index_values.lower_bound(end);
			iter = begin_iter;
			while (iter != end_iter)
			{
				if (predicative(iter->second, result))
					result = iter->second;
				++iter;
			}
			return result;
		}

		value_type max(index_type begin, index_type end) const
		{
			return best(begin, end, [](value_type a, value_type b)->bool {return a > b; });
		}

		value_type min(index_type begin, index_type end) const
		{
			return best(begin, end, [](value_type a, value_type b)->bool {return a < b; });
		}

	private:
		value_type i_leftmost;
	};

	template <typename index_type, typename value_type>
	class linear_interpolator : public interpolator<index_type, value_type>
	{
		using interpolator<index_type, value_type>::index_values;

	public:
		using interpolator<index_type, value_type>::interpolator;

		value_type get(index_type index) const override
		{
			using const_iterator = typename std::map<index_type, value_type>::const_iterator;
			const_iterator iter_right, iter_left;
			iter_right = index_values.upper_bound(index);
			if (iter_right == index_values.begin())
			{
				iter_left = iter_right;
				++iter_right;
			}
			else if (iter_right == index_values.end())
			{
				--iter_right;
				iter_left = iter_right;
				--iter_left;
			}
			else
			{
				iter_left = iter_right;
				--iter_left;
			}
			return linear(iter_left->first, iter_left->second, iter_right->first, iter_right->second, index);
		}

		template <typename fun_type>
		value_type best(index_type begin, index_type end, fun_type predicative) const
		{
			using const_iterator = typename std::map<index_type, value_type>::const_iterator;
			const_iterator begin_iter, end_iter;
			value_type result{}, temp_result{};
			if (end < begin)
				throw end_less_than_begin();
			result = get(begin);
			if (begin < end)
			{
				temp_result = get(end);
				if (predicative(temp_result, result))
					result = temp_result;
				begin_iter = index_values.upper_bound(begin);
				end_iter = index_values.lower_bound(end);
				for (const_iterator iter = begin_iter; iter != end_iter; ++iter)
					if (predicative(iter->second, result))
						result = iter->second;
			}
			return result;
		}

		value_type max(index_type begin, index_type end) const
		{
			return best(begin, end, [](value_type a, value_type b)->bool {return a > b; });
		}

		value_type min(index_type begin, index_type end) const
		{
			return best(begin, end, [](value_type a, value_type b)->bool {return a < b; });
		}

	private:
		static value_type linear(index_type x1, value_type y1, index_type x2, value_type y2, index_type x)
		{
#ifdef LINEAR_DIVISION_FIRST
			return y1 + (y1 - y2) / (x1 - x2) * (x - x1);
#else
			return y1 + (x - x1) * (y1 - y2) / (x1 - x2);
#endif
		}
	};

	template <typename index_type, typename value_type>
	class quartic_interpolator : public interpolator<index_type, value_type>
	{
		using interpolator<index_type, value_type>::index_values;

	public:
		using interpolator<index_type, value_type>::interpolator;

		value_type get(index_type index) const override
		{
			using const_iterator = typename std::map<index_type, value_type>::const_iterator;
			const_iterator iter1, iter2, iter3, iter4;
			iter3 = index_values.upper_bound(index);
			if (iter3 == index_values.begin())
			{
				iter2 = iter3;
				++iter3;
			}
			else if (iter3 == index_values.end())
			{
				--iter3;
				iter2 = iter3;
				--iter2;
			}
			else
			{
				iter2 = iter3;
				--iter2;
			}
			iter1 = iter2;
			if (iter2 != index_values.begin())
				--iter1;
			iter4 = iter3;
			++iter4;
			if (iter4 == index_values.end())
				--iter4;
			return quartic(
				iter1->first, iter1->second,
				iter2->first, iter2->second,
				iter3->first, iter3->second,
				iter4->first, iter4->second,
				index);
		}

	private:
		static value_type quartic(
			index_type x1, value_type y1,
			index_type x2, value_type y2,
			index_type x3, value_type y3,
			index_type x4, value_type y4,
			index_type x)
		{
			return
#ifdef QUARTIC_DIVISION_FIRST
				y2 * (1.0 + (x - x2) / (x3 - x2) * 2.0) * sq((x - x3) / (x2 - x3)) +
				y3 * (1.0 + (x - x3) / (x2 - x3) * 2.0) * sq((x - x2) / (x3 - x2)) +
				(y3 - y1) / (x3 - x1) * (x - x2) * sq((x - x3) / (x2 - x3)) +
				(y4 - y2) / (x4 - x2) * (x - x3) * sq((x - x2) / (x3 - x2));
#else
				y2 * (1.0 + 2.0 * (x - x2) / (x3 - x2)) * sq((x - x3) / (x2 - x3)) +
				y3 * (1.0 + 2.0 * (x - x3) / (x2 - x3)) * sq((x - x2) / (x3 - x2)) +
				(x - x2) * sq((x - x3) / (x2 - x3)) * (y3 - y1) / (x3 - x1) +
				(x - x3) * sq((x - x2) / (x3 - x2)) * (y4 - y2) / (x4 - x2);
#endif
		}

		template <typename T>
		static T sq(T num)
		{
			return num * num;
		}
	};

	template <typename index_type, typename value_type>
	class polynomial_interpolator : public interpolator<index_type, value_type>
	{
		using interpolator<index_type, value_type>::index_values;

	public:
		using interpolator<index_type, value_type>::interpolator;

		value_type get(index_type index) const override // Using Lagrange interpolation formula
		{
			value_type product = 0.0, sum = 0.0;
			for (const std::pair<index_type, value_type>& index_value : index_values)
			{
				product = index_value.second;
				for (const std::pair<index_type, value_type>& index_value_2 : index_values)
				{
					if (index_value.first == index_value_2.first)
						continue;
					product *= (index - index_value_2.first) / (index_value.first - index_value_2.first);
				}
				sum += product;
			}
			return sum;
		}
	};

	template <typename index_type, typename value_type>
	class single_sine_interpolator : public interpolator<index_type, value_type>
	{
		using interpolator<index_type, value_type>::index_values;

	public:
		using interpolator<index_type, value_type>::interpolator;

		value_type get(index_type index) const override
		{
			using const_iterator = typename std::map<index_type, value_type>::const_iterator;
			const_iterator iter_right, iter_left;
			iter_right = index_values.upper_bound(index);
			if (iter_right == index_values.begin())
			{
				iter_left = iter_right;
				++iter_right;
			}
			else if (iter_right == index_values.end())
			{
				--iter_right;
				iter_left = iter_right;
				--iter_left;
			}
			else
			{
				iter_left = iter_right;
				--iter_left;
			}
			return single_sine(iter_left->first, iter_left->second, iter_right->first, iter_right->second, index);
		}

		template <typename fun_type>
		value_type best(index_type begin, index_type end, fun_type predicative) const
		{
			using const_iterator = typename std::map<index_type, value_type>::const_iterator;
			const_iterator begin_iter, end_iter;
			value_type result{}, temp_result{};
			if (end < begin)
				throw end_less_than_begin();
			result = get(begin);
			if (begin < end)
			{
				temp_result = get(end);
				if (predicative(temp_result, result))
					result = temp_result;
				begin_iter = index_values.upper_bound(begin);
				end_iter = index_values.lower_bound(end);
				for (const_iterator iter = begin_iter; iter != end_iter; ++iter)
					if (predicative(iter->second, result))
						result = iter->second;
			}
			return result;
		}

		value_type max(index_type begin, index_type end) const
		{
			return best(begin, end, [](value_type a, value_type b)->bool {return a > b; });
		}

		value_type min(index_type begin, index_type end) const
		{
			return best(begin, end, [](value_type a, value_type b)->bool {return a < b; });
		}

	private:
		static value_type single_sine(index_type x1, value_type y1, index_type x2, value_type y2, index_type x)
		{
			if (x < x1)
				return y1;
			else if (x2 < x)
				return y2;
			else
				return (1.0 - exact_cos((x - x1) / (x2 - x1))) / 2.0 * (y2 - y1) + y1;
		}
	};

	template <typename index_type, typename value_type>
	class sinc_interpolator : public interpolator<index_type, value_type>
	{
		using interpolator<index_type, value_type>::index_values;

	public:
		using interpolator<index_type, value_type>::set;

		sinc_interpolator(index_type interval = 1.0) : i_interval(interval) {}

		sinc_interpolator(const std::map<index_type, value_type>& table, index_type interval = 1.0) :
			i_interval(interval)
		{
			set(table);
		}

		void interval(index_type value)
		{
			i_interval = value;
		}

		value_type get(index_type index) const override
		{
			value_type result = 0.0;
			for (std::pair<index_type, value_type> index_value : index_values)
				result += sinc((index - index_value.first) / i_interval) * index_value.second;
			return result;
		}

	private:
		static double sinc(double index)
		{
			if (index == 0)
				return 1.0;
			else
				return exact_sin(index) / (index * PI);
		}

		index_type i_interval;
	};

	template <typename index_type, typename value_type>
	class sine_period_interpolator : public interpolator<index_type, value_type>
	{
		using interpolator<index_type, value_type>::index_values;

	public:
		using interpolator<index_type, value_type>::set;

		sine_period_interpolator(unsigned period = 1U, index_type interval = 1.0) :
			i_period(period), i_interval(interval)
		{}

		sine_period_interpolator(const std::map<index_type, value_type>& table, unsigned period = 1U, index_type interval = 1.0) :
			i_period(period), i_interval(interval)
		{
			set(table);
		}

		void period(unsigned value)
		{
			i_period = value;
		}

		void interval(index_type value)
		{
			i_interval = value;
		}

		value_type get(index_type index) const override
		{
			value_type result{};
			for (const std::pair<index_type, value_type>& index_value : index_values)
				result += periodic_sampling(i_period, (index - index_value.first) / i_interval) * index_value.second;
			return result;
		}

	private:
		static double periodic_sampling(unsigned n, double index)
		{
			double result = 0.0;
			for (unsigned i = 0; i <= n / 2; ++i)
				if (i == 0)
					result += 1.0;
				else if (i * 2 == n)
					result += exact_cos(index);
				else
					result += exact_cos(i * 2.0 / n * index) * 2.0;
			result /= n;
			return result;
		}

		unsigned i_period;
		index_type i_interval;
	};
}
