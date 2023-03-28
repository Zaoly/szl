#pragma once

namespace zaoly
{
	template <typename number, number modulo>
	class modint
	{
	public:
		modint(number num = 0) noexcept
		{
			set(num);
		}

		void set(number num) noexcept
		{
			if (num >= 0)
				m_val = num % modulo;
			else
				m_val = num % modulo + modulo;
		}

		modint& operator=(number num) noexcept
		{
			set(num);
			return *this;
		}

		modint& operator++() noexcept
		{
			++m_val;
			if (m_val >= modulo)
				m_val = 0;
			return *this;
		}

		modint operator++(int) noexcept
		{
			modint result = *this;
			m_val++;
			if (m_val >= modulo)
				m_val = 0;
			return result;
		}

		modint& operator--() noexcept
		{
			--m_val;
			if (m_val >= modulo)
				m_val = 0;
			return *this;
		}

		modint operator--(int) noexcept
		{
			modint result = *this;
			m_val--;
			if (m_val >= modulo)
				m_val = 0;
			return result;
		}

		template <typename T>
		modint& operator+=(T num) noexcept
		{
			set(m_val + num);
			return *this;
		}

		template <typename T>
		modint& operator-=(T num) noexcept
		{
			set(m_val - num);
			return *this;
		}

		template <typename T>
		modint& operator*=(T num) noexcept
		{
			set(m_val * num);
			return *this;
		}

		template <typename T>
		modint& operator/=(T num) noexcept
		{
			set(m_val / num);
			return *this;
		}

		template <typename T>
		modint& operator%=(T num) noexcept
		{
			set(m_val % num);
			return *this;
		}

		template <typename T>
		modint& operator&=(T num) noexcept
		{
			set(m_val & num);
			return *this;
		}

		template <typename T>
		modint& operator|=(T num) noexcept
		{
			set(m_val | num);
			return *this;
		}

		template <typename T>
		modint& operator^=(T num) noexcept
		{
			set(m_val ^ num);
			return *this;
		}

		template <typename T>
		modint& operator<<=(T num) noexcept
		{
			set(m_val << num);
			return *this;
		}

		template <typename T>
		modint& operator>>=(T num) noexcept
		{
			set(m_val >> num);
			return *this;
		}

		operator number() const noexcept
		{
			return m_val;
		}

	private:
		number m_val;
	};
}
