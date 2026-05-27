#pragma once

#include <exception>
#include <iostream>

namespace bmstu
{
template <typename T>
class basic_string;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;

template <typename T>
class basic_string
{
   private:
	static constexpr size_t SSO_CAPACITY =
		(sizeof(T*) + sizeof(size_t) + sizeof(size_t)) / sizeof(T) - 1;

	struct LongString
	{
		T* ptr;
		size_t size;
		size_t capacity;
	};

	struct ShortString
	{
		T buffer[SSO_CAPACITY + 1];
		unsigned char size;
	};

	union Data
	{
		LongString long_str;
		ShortString short_str;
	};

	Data data_;
	bool is_long_;

	bool is_long() const { return is_long_; }

	// Private Getters

	T* get_ptr()
	{
		if (is_long_)
			return data_.long_str.ptr;
		else
			return data_.short_str.buffer;
	}

	const T* get_ptr() const
	{
		if (is_long_)
			return data_.long_str.ptr;
		else
			return data_.short_str.buffer;
	}

	size_t get_size() const
	{
		if (is_long_)
			return data_.long_str.size;
		else
			return data_.short_str.size;
	}

	size_t get_capacity() const
	{
		if (is_long_)
			return data_.long_str.capacity;
		else
			return SSO_CAPACITY;
	}

   public:
	// DefaultConstructor
	basic_string()
	{
		is_long_ = false;
		data_.short_str.size = 0;
		data_.short_str.buffer[0] = T{0};
	}

	// ConstructorSize
	basic_string(size_t size)
	{
		if (size <= SSO_CAPACITY)
		{
			is_long_ = false;
			data_.short_str.size = static_cast<unsigned char>(size);
			for (size_t i = 0; i < size; i++)
			{
				data_.short_str.buffer[i] = T(' ');
			}
			data_.short_str.buffer[size] = T{0};
		}
		else
		{
			is_long_ = true;
			data_.long_str.size = size;
			data_.long_str.capacity = size;
			data_.long_str.ptr = new T[size + 1];
			for (size_t i = 0; i < size; i++)
			{
				data_.long_str.ptr[i] = T(' ');
			}
			data_.long_str.ptr[size] = T{0};
		}
	}

	// Constructor list
	basic_string(std::initializer_list<T> il)
	{
		size_t len = il.size();

		if (len <= SSO_CAPACITY)
		{
			is_long_ = false;
			data_.short_str.size = static_cast<unsigned char>(len);

			size_t i = 0;
			for (const T& symbol : il)
			{
				data_.short_str.buffer[i++] = symbol;
			}
			data_.short_str.buffer[len] = T{0};
		}
		else
		{
			is_long_ = true;
			data_.long_str.size = len;
			data_.long_str.capacity = len;
			data_.long_str.ptr = new T[len + 1];

			size_t i = 0;
			for (const T& symbol : il)
			{
				data_.long_str.ptr[i++] = symbol;
			}
			data_.long_str.ptr[len] = T{0};
		}
	}

	// Constructor C-str
	basic_string(const T* c_str)
	{
		size_t len = strlen_(c_str);

		if (len <= SSO_CAPACITY)
		{
			is_long_ = false;
			data_.short_str.size = static_cast<unsigned char>(len);
			for (size_t i = 0; i < len; i++)
			{
				data_.short_str.buffer[i] = c_str[i];
			}
			data_.short_str.buffer[len] = T{0};
		}
		else
		{
			is_long_ = true;
			data_.long_str.size = len;
			data_.long_str.capacity = len;
			data_.long_str.ptr = new T[len + 1];
			for (size_t i = 0; i < len; i++)
			{
				data_.long_str.ptr[i] = c_str[i];
			}
			data_.long_str.ptr[len] = T{0};
		}
	}

	// Copy Constructor
	basic_string(const basic_string& other)
	{
		is_long_ = other.is_long_;
		if (!is_long_)
		{
			data_.short_str.size = other.data_.short_str.size;
			for (size_t i = 0; i < other.data_.short_str.size; i++)
			{
				data_.short_str.buffer[i] = other.data_.short_str.buffer[i];
			}
			data_.short_str.buffer[other.data_.short_str.size] = T{0};
		}
		else
		{
			data_.long_str.ptr = new T[other.data_.long_str.size + 1];
			data_.long_str.size = other.data_.long_str.size;
			data_.long_str.capacity = other.data_.long_str.capacity;
			for (size_t i = 0; i < other.data_.long_str.size; i++)
			{
				data_.long_str.ptr[i] = other.data_.long_str.ptr[i];
			}
			data_.long_str.ptr[other.data_.long_str.size] = T{0};
		}
	}

	// Move Constructor
	basic_string(basic_string&& dying) noexcept
	{
		is_long_ = dying.is_long_;
		if (!is_long_)
		{
			data_.short_str.size = dying.data_.short_str.size;
			for (size_t i = 0; i < dying.data_.short_str.size; i++)
			{
				data_.short_str.buffer[i] = dying.data_.short_str.buffer[i];
			}
			data_.short_str.buffer[dying.data_.short_str.size] = T{0};

			dying.data_.short_str.size = 0;
		}
		else
		{
			data_.long_str.ptr = dying.data_.long_str.ptr;
			data_.long_str.size = dying.data_.long_str.size;
			data_.long_str.capacity = dying.data_.long_str.capacity;

			dying.data_.long_str.ptr = nullptr;
			dying.data_.long_str.size = 0;
			dying.data_.long_str.capacity = 0;
		}
	}

	// Destructor
	~basic_string() { clean_(); }

	// Getters
	const T* c_str() const { return get_ptr(); }

	size_t size() const { return get_size(); }

	bool is_using_sso() const { return !(is_long_); }

	size_t capacity() const { return get_capacity(); }

	// Operators
	basic_string& operator=(basic_string&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		clean_();

		is_long_ = other.is_long_;
		if (!is_long_)
		{
			data_.short_str.size = other.data_.short_str.size;
			for (size_t i = 0; i < other.data_.short_str.size; i++)
			{
				data_.short_str.buffer[i] = other.data_.short_str.buffer[i];
			}
			data_.short_str.buffer[other.data_.short_str.size] = T{0};

			other.data_.short_str.size = 0;
			other.data_.short_str.buffer[0] = T{0};
		}
		else
		{
			data_.long_str.ptr = other.data_.long_str.ptr;
			data_.long_str.size = other.data_.long_str.size;
			data_.long_str.capacity = other.data_.long_str.capacity;

			other.data_.long_str.ptr = nullptr;
			other.data_.long_str.size = 0;
			other.data_.long_str.capacity = 0;
		}

		return *this;
	}

	basic_string& operator=(const T* c_str)
	{
		clean_();

		size_t len = strlen_(c_str);

		if (len <= SSO_CAPACITY)
		{
			is_long_ = false;
			data_.short_str.size = static_cast<unsigned char>(len);
			for (size_t i = 0; i < len; i++)
			{
				data_.short_str.buffer[i] = c_str[i];
			}
			data_.short_str.buffer[len] = T{0};
		}
		else
		{
			is_long_ = true;
			data_.long_str.size = len;
			data_.long_str.capacity = len;
			data_.long_str.ptr = new T[len + 1];
			for (size_t i = 0; i < len; i++)
			{
				data_.long_str.ptr[i] = c_str[i];
			}
			data_.long_str.ptr[len] = T{0};
		}

		return *this;
	}

	basic_string& operator=(const basic_string& other)
	{
		if (this == &other)
		{
			return *this;
		}

		clean_();

		is_long_ = other.is_long_;
		if (!is_long_)
		{
			data_.short_str.size = other.data_.short_str.size;
			for (size_t i = 0; i < other.data_.short_str.size; i++)
			{
				data_.short_str.buffer[i] = other.data_.short_str.buffer[i];
			}
			data_.short_str.buffer[other.data_.short_str.size] = T{0};
		}
		else
		{
			data_.long_str.ptr = new T[other.data_.long_str.size + 1];
			data_.long_str.size = other.data_.long_str.size;
			data_.long_str.capacity = other.data_.long_str.capacity;
			for (size_t i = 0; i < other.data_.long_str.size; i++)
			{
				data_.long_str.ptr[i] = other.data_.long_str.ptr[i];
			}
			data_.long_str.ptr[other.data_.long_str.size] = T{0};
		}

		return *this;
	}

	friend basic_string<T> operator+(const basic_string<T>& left,
									 const basic_string<T>& right)
	{
		basic_string<T> result = left;
		result += right;
		return result;
	}

	template <typename S>
	friend S& operator<<(S& os, const basic_string& obj)
	{
		const T* new_ptr = obj.get_ptr();
		size_t new_size = obj.size();

		for (size_t i = 0; i < new_size; i++)
		{
			os << new_ptr[i];
		}

		return os;
	}

	template <typename S>
	friend S& operator>>(S& is, basic_string& obj)
	{
		obj.clean_();

		T symbol;
		while (is.get(symbol))
		{
			obj += symbol;
		}

		return is;
	}

	basic_string& operator+=(const basic_string& other)
	{
		size_t old_len = get_size();
		size_t new_len = old_len + other.size();

		if (new_len <= get_capacity())
		{
			for (size_t i = 0; i < other.size(); i++)
			{
				get_ptr()[old_len + i] = other.get_ptr()[i];
			}
			get_ptr()[new_len] = T{0};

			if (is_long_)
			{
				data_.long_str.size = new_len;
			}
			else
			{
				data_.short_str.size = static_cast<unsigned char>(new_len);
			}
		}
		else
		{
			T* new_ptr = new T[new_len + 1];

			for (size_t i = 0; i < old_len; i++)
			{
				new_ptr[i] = get_ptr()[i];
			}

			for (size_t i = 0; i < other.size(); i++)
			{
				new_ptr[old_len + i] = other.get_ptr()[i];
			}

			new_ptr[new_len] = T{0};

			clean_();
			is_long_ = true;
			data_.long_str.size = new_len;
			data_.long_str.capacity = new_len;
			data_.long_str.ptr = new_ptr;
		}

		return *this;
	}

	basic_string& operator+=(T symbol)
	{
		size_t old_len = get_size();
		size_t new_len = old_len + 1;

		if (new_len <= get_capacity())
		{
			get_ptr()[old_len] = symbol;
			get_ptr()[new_len] = T{0};

			if (is_long_)
			{
				data_.long_str.size = new_len;
			}
			else
			{
				data_.short_str.size = static_cast<unsigned char>(new_len);
			}
		}
		else
		{
			T* new_ptr = new T[new_len + 1];

			for (size_t i = 0; i < old_len; i++)
			{
				new_ptr[i] = get_ptr()[i];
			}

			new_ptr[old_len] = symbol;
			new_ptr[new_len] = T{0};

			clean_();
			is_long_ = true;
			data_.long_str.size = new_len;
			data_.long_str.capacity = new_len;
			data_.long_str.ptr = new_ptr;
		}

		return *this;
	}

	// Get index
	T& operator[](size_t index) noexcept { return get_ptr()[index]; }

	T& at(size_t index)
	{
		if (index >= size())
		{
			throw std::out_of_range("Wrong index");
		}
		else
		{
			return get_ptr()[index];
		}
	}

	T* data() { return get_ptr(); }

   private:
	static size_t strlen_(const T* str)
	{
		size_t len = 0;

		while (str[len] != '\0')
		{
			++len;
		}

		return len;
	}

	void clean_()
	{
		if (is_long_)
		{
			delete[] data_.long_str.ptr;
			data_.long_str.ptr = nullptr;
			data_.long_str.size = 0;
			data_.long_str.capacity = 0;
			is_long_ = false;
		}
		else
		{
			data_.short_str.size = 0;
		}
	}
};
}  // namespace bmstu
