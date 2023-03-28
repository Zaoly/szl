#pragma once

#include <utility>

namespace zaoly
{
	constexpr int COPY = 0;

	template <typename T>
	class dependent_ptr // Dependent pointer, point to an existing data or copy and store data
	{
	public:
		dependent_ptr(T* data_ptr = nullptr) : is_newed(false) // Point to an existing data
		{
			point(data_ptr);
		}

		dependent_ptr(const dependent_ptr& dp)
		{
			is_newed = false;
			copy_assign(dp);
		}

		dependent_ptr(dependent_ptr&& dp)
		{
			is_newed = false;
			move_assign(std::move(dp));
		}

		~dependent_ptr()
		{
			free();
		}

		T* get() const
		{
			return d_ptr;
		}

		void point(T* data_ptr)
		{
			free();
			d_ptr = data_ptr;
			is_newed = false;
		}

		template <typename R = T>
		void copy(const R& data_ref)
		{
			free();
			d_ptr = static_cast<T*>(new R(data_ref));
			is_newed = true;
		}

		dependent_ptr& operator=(T* data_ptr)
		{
			point(data_ptr);
			return *this;
		}

		dependent_ptr& operator=(const dependent_ptr& dp)
		{
			copy_assign(dp);
			return *this;
		}

		dependent_ptr& operator=(dependent_ptr&& dp)
		{
			move_assign(std::move(dp));
			return *this;
		}

		T& operator*() const
		{
			return *d_ptr;
		}

		T* operator->() const
		{
			return d_ptr;
		}

		T& operator[](ptrdiff_t index) const
		{
			return d_ptr[index];
		}

		operator T* () const
		{
			return d_ptr;
		}

	private:
		void copy_assign(const dependent_ptr& dp)
		{
			free();
			is_newed = dp.is_newed;
			if (is_newed)
				d_ptr = new T(dp.d_ptr);
			else
				d_ptr = dp.d_ptr;
		}

		void move_assign(dependent_ptr&& dp)
		{
			free();
			is_newed = dp.is_newed;
			if (is_newed)
				std::swap(d_ptr, dp.d_ptr);
			else
				d_ptr = dp.d_ptr;
		}

		void free() const
		{
			if (is_newed)
				delete d_ptr;
		}

		T* d_ptr;
		bool is_newed; // If true, copying and restoring data; if false, pointing to other data
	};
}
