#pragma once

#include <cmath>
#include <iostream>

namespace zaoly
{
	template <typename number>
	class operblock // Operation block, combining formulas like building blocks
	{
	public:
		virtual constexpr const char* name() const = 0;

		virtual number eval() const = 0;

		virtual void print(std::ostream&, const char*, const char*) const = 0;

		virtual number print_chain(std::ostream&, const char*, const char*) const = 0;

		virtual ~operblock() {}
	};

	template <typename number>
	class operblock_num : public operblock<number> // The most basic block, representing pure number
	{
	public:
		operblock_num(number num = {}) : n_num(num) {}

		constexpr const char* name() const override
		{
			return "number";
		}

		number eval() const override
		{
			return n_num;
		}

		operblock_num& operator=(const operblock_num& bn)
		{
			n_num = bn.n_num;
			return *this;
		}

		operblock_num& operator=(number num)
		{
			n_num = num;
			return *this;
		}

		explicit operator number() const
		{
			return n_num;
		}

	private:
		void print(std::ostream& = std::cout, const char* = "", const char* = "\n") const override
		{}

		number print_chain(std::ostream& = std::cout, const char* = "", const char* = "\n") const override
		{
			return eval();
		}

		number n_num;
	};

	template <typename number, unsigned operand_count>
	class operblock_base : public operblock<number> // Operation that receives operand_count operand(s)
	{
	public:
		operblock<number>* operands[operand_count]{};
	};

	template <typename number>
	class operblock_double : public operblock_base<number, 2> // Operation that receives 2 operands
	{
		using operblock<number>::eval;
		using operblock_base<number, 2>::operands;

	public:
		operblock_double(operblock<number>* a = nullptr, operblock<number>* b = nullptr)
		{
			operands[0] = a;
			operands[1] = b;
		}

		virtual constexpr const char* sign() const = 0;

		void print(std::ostream& os = std::cout, const char* delimeter1 = "", const char* delimeter2 = "\n") const override
		{
			os << operands[0]->eval() << delimeter1 << sign() << delimeter1 << operands[1]->eval() << delimeter1 << '=' << delimeter1 << eval() << delimeter2;
		}

		number print_chain(std::ostream& os = std::cout, const char* delimeter1 = "", const char* delimeter2 = "\n") const override
		{
			number a{}, b{}, c{};
			a = operands[0]->print_chain(os, delimeter1, delimeter2);
			b = operands[1]->print_chain(os, delimeter1, delimeter2);
			c = eval();
			os << a << delimeter1 << sign() << delimeter1 << b << delimeter1 << '=' << delimeter1 << c << delimeter2;
			return c;
		}
	};

	template <typename number>
	class operblock_add : public operblock_double<number>
	{
	public:
		using operblock_base<number, 2>::operands;
		using operblock_double<number>::operblock_double;

		constexpr const char* name() const override
		{
			return "add";
		}

		number eval() const override
		{
			return operands[0]->eval() + operands[1]->eval();
		}

		constexpr const char* sign() const
		{
			return "+";
		}
	};

	template <typename number>
	class operblock_subtract : public operblock_double<number>
	{
	public:
		using operblock_base<number, 2>::operands;
		using operblock_double<number>::operblock_double;

		constexpr const char* name() const override
		{
			return "subtract";
		}

		number eval() const override
		{
			return operands[0]->eval() - operands[1]->eval();
		}

		constexpr const char* sign() const
		{
			return "-";
		}
	};

	template <typename number>
	class operblock_multiply : public operblock_double<number>
	{
	public:
		using operblock_base<number, 2>::operands;
		using operblock_double<number>::operblock_double;

		constexpr const char* name() const override
		{
			return "multiply";
		}

		number eval() const override
		{
			return operands[0]->eval() * operands[1]->eval();
		}

		constexpr const char* sign() const
		{
			return "*";
		}
	};

	template <typename number>
	class operblock_divide : public operblock_double<number>
	{
	public:
		using operblock_base<number, 2>::operands;
		using operblock_double<number>::operblock_double;

		constexpr const char* name() const override
		{
			return "divide";
		}

		number eval() const override
		{
			return operands[0]->eval() / operands[1]->eval();
		}

		constexpr const char* sign() const
		{
			return "/";
		}
	};

	template <typename number>
	class operblock_pow : public operblock_double<number>
	{
	public:
		using operblock_base<number, 2>::operands;
		using operblock_double<number>::operblock_double;

		constexpr const char* name() const override
		{
			return "power";
		}

		number eval() const override
		{
			return pow(operands[0]->eval(), operands[1]->eval());
		}

		constexpr const char* sign() const
		{
			return "^";
		}
	};

	template <typename number>
	class operblock_single : public operblock_base<number, 1> // Operation that receives 1 operand
	{
		using operblock<number>::name;
		using operblock<number>::eval;
		using operblock_base<number, 1>::operands;

	public:
		operblock_single(operblock<number>* operand = nullptr)
		{
			operands[0] = operand;
		}

		virtual constexpr const char* sign() const = 0;

		void print(std::ostream& os = std::cout, const char* delimeter1 = "", const char* delimeter2 = "\n") const override
		{
			os << sign() << delimeter1 << operands[0]->eval() << delimeter1 << '=' << delimeter1 << eval() << delimeter2;
		}

		number print_chain(std::ostream& os = std::cout, const char* delimeter1 = "", const char* delimeter2 = "\n") const override
		{
			number operand{}, result{};
			operand = operands[0]->print_chain(os, delimeter1, delimeter2);
			result = eval();
			os << sign() << delimeter1 << operand << delimeter1 << '=' << delimeter1 << result << delimeter2;
			return result;
		}
	};

	template <typename number>
	class operblock_sin : public operblock_single<number>
	{
	public:
		using operblock_base<number, 1>::operands;
		using operblock_single<number>::operblock_single;

		constexpr const char* name() const override
		{
			return "sine";
		}

		number eval() const override
		{
			return sin(operands[0]->eval());
		}

		constexpr const char* sign() const override
		{
			return "sin";
		}
	};

	template <typename number>
	class operblock_cos : public operblock_single<number>
	{
	public:
		using operblock_base<number, 1>::operands;
		using operblock_single<number>::operblock_single;

		constexpr const char* name() const override
		{
			return "cosine";
		}

		number eval() const override
		{
			return cos(operands[0]->eval());
		}

		constexpr const char* sign() const override
		{
			return "cos";
		}
	};

	template <typename number>
	class operblock_tan : public operblock_single<number>
	{
	public:
		using operblock_base<number, 1>::operands;
		using operblock_single<number>::operblock_single;

		constexpr const char* name() const override
		{
			return "tangent";
		}

		number eval() const override
		{
			return tan(operands[0]->eval());
		}

		constexpr const char* sign() const override
		{
			return "tan";
		}
	};

	template <typename number>
	class operblock_log : public operblock_single<number>
	{
	public:
		using operblock_base<number, 1>::operands;
		using operblock_single<number>::operblock_single;

		constexpr const char* name() const override
		{
			return "logarithm";
		}

		number eval() const override
		{
			return log(operands[0]->eval());
		}

		constexpr const char* sign() const override
		{
			return "log";
		}
	};
}
