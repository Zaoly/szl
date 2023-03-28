#pragma once

#include <functional>
#include <map>
#include <unordered_map>
#include <vector>

namespace zaoly
{
	using void_fun_t = std::function<void()>;

	template <typename key_type, typename funs_type>
	class equal_switcher // Common template for switcher and unordered switcher
	{
	public:
		void insert(const key_type& value, const void_fun_t& fun)
		{
			funs.insert(std::pair<key_type, void_fun_t>(value, fun));
		}

		void set_default(const void_fun_t& fun)
		{
			default_fun = fun;
		}

		void execute(const key_type& value) const
		{
			if (funs.find(value) != funs.end())
				funs.at(value)();
			else if (default_fun)
				default_fun();
		}

		void clear()
		{
			funs.clear();
			default_fun = nullptr;
		}

	protected:
		equal_switcher() {}
		equal_switcher(const equal_switcher&) = delete;
		equal_switcher(equal_switcher&&) = delete;

	private:
		funs_type funs;
		void_fun_t default_fun;
	};

	template <typename key_type>
	class switcher : public equal_switcher // Using ordered map in STL
	<
		key_type,
		std::map<key_type, void_fun_t>
	>
	{};

	template <typename key_type>
	class unordered_switcher : public equal_switcher // Using unordered map in STL
	<
		key_type,
		std::unordered_map<key_type, void_fun_t>
	>
	{};

	template <typename key_type>
	class range_switcher // Range is always "a <= value <= b", always using operator "<"
	{
		using void_fun_key_t = std::function<void(key_type)>;

	public:
		range_switcher() {}
		range_switcher(const range_switcher&) = delete;
		range_switcher(range_switcher&&) = delete;

		void insert(const key_type& left, const key_type& right, const void_fun_key_t& fun)
		{
			rfs.emplace_back(left, right, fun);
		}

		void insert(const key_type& value, const void_fun_key_t& fun)
		{
			rfs.emplace_back(value, value, fun);
		}

		void set_default(const void_fun_key_t& fun)
		{
			default_fun = fun;
		}

		void execute(const key_type& value) const
		{
			for (const range_function& rf : rfs)
				if (!(value < rf.left || rf.right < value))
				{
					rf.fun(value);
					return;
				}
			if (default_fun)
				default_fun(value);
		}

		void clear()
		{
			rfs.clear();
			default_fun = nullptr;
		}

	private:
		struct range_function
		{
			key_type left, right;
			void_fun_key_t fun;

			range_function(key_type _left, key_type _right, void_fun_key_t _fun) :
				left(_left),
				right(_right),
				fun(_fun)
			{}
		};

		std::vector<range_function> rfs;
		void_fun_key_t default_fun;
	};

	template <typename key_type>
	class conditional_switcher // Just like "if ... else if ... else ..." structure, but only one variable is judged
	{
		using bool_fun_key_t = std::function<bool(key_type)>;
		using void_fun_key_t = std::function<void(key_type)>;

	public:
		conditional_switcher() {}
		conditional_switcher(const conditional_switcher&) = delete;
		conditional_switcher(conditional_switcher&&) = delete;

		void insert(const bool_fun_key_t& cond, const void_fun_key_t& fun)
		{
			cfs.emplace_back(cond, fun);
		}

		void set_default(const void_fun_key_t& fun)
		{
			default_fun = fun;
		}

		void execute(const key_type& value) const
		{
			for (const condition_function& cf : cfs)
				if (cf.cond(value))
				{
					cf.fun(value);
					return;
				}
			if (default_fun)
				default_fun(value);
		}

		void clear()
		{
			cfs.clear();
			default_fun = nullptr;
		}

	private:
		struct condition_function
		{
			bool_fun_key_t cond;
			void_fun_key_t fun;

			condition_function(bool_fun_key_t _cond, void_fun_key_t _fun) :
				cond(_cond),
				fun(_fun)
			{}
		};

		std::vector<condition_function> cfs;
		void_fun_key_t default_fun;
	};
}
