#include <ostream>
#include <stdexcept>
#include <utility>
#include "array_ptr.h"

namespace bmstu
{
template <typename T>
class simple_vector
{
   public:
	class iterator
	{
	   public:
		using iterator_category = std::contiguous_iterator_tag;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using difference_type = std::ptrdiff_t;

		iterator() = default;

		iterator(const iterator& other) = default;

		iterator(std::nullptr_t) noexcept : ptr_(nullptr) {}

		iterator(iterator&& other) noexcept : ptr_(nullptr) {}

		explicit iterator(pointer ptr) : ptr_(ptr) {}

		reference operator*() const { return *ptr_; }

		pointer operator->() const { return ptr_; }

		friend pointer to_address(const iterator& it) noexcept
		{
			return it.ptr_;
		}

		iterator& operator=(const iterator& other) = default;

		iterator& operator=(iterator&& other) noexcept { return *this; }

#pragma region Operators
		iterator& operator++()
		{
			++ptr_;
			return *this;
		}

		iterator& operator--()
		{
			--ptr_;
			return *this;
		}

		iterator operator++(int)
		{
			iterator temp = *this;
			++ptr_;
			return temp;
		}

		iterator operator--(int)
		{
			iterator temp = *this;
			--ptr_;
			return temp;
		}

		explicit operator bool() const { return ptr_ != nullptr; }

		friend bool operator==(const iterator& lhs, const iterator& rhs)
		{
			if (lhs.ptr_ == rhs.ptr_)
			{
				return true;
			}

			return false;
		}

		friend bool operator==(const iterator& lhs, std::nullptr_t)
		{
			if (lhs.ptr_ == nullptr)
			{
				return true;
			}
			return false;
		}

		iterator& operator=(std::nullptr_t) noexcept
		{
			ptr_ = nullptr;
			return *this;
		}

		friend bool operator==(std::nullptr_t, const iterator& rhs)
		{
			if (nullptr == rhs.ptr_)
			{
				return true;
			}
			return false;
		}

		friend bool operator!=(const iterator& lhs, const iterator& rhs)
		{
			if (lhs.ptr_ == rhs.ptr_)
			{
				return false;
			}
			return true;
		}

		iterator operator+(const difference_type& n) const noexcept
		{
			return iterator(ptr_ + n);
		}

		iterator operator+=(const difference_type& n) noexcept
		{
			ptr_ += n;
			return *this;
		}

		iterator operator-(const difference_type& n) const noexcept
		{
			return iterator(ptr_ - n);
		}

		iterator operator-=(const difference_type& n) noexcept
		{
			ptr_ -= n;
			return *this;
		}

		friend difference_type operator-(const iterator& end,
										 const iterator& begin) noexcept
		{
			return end.ptr_ - begin.ptr_;
		}

#pragma endregion
	   private:
		pointer ptr_ = nullptr;
	};

	simple_vector() noexcept = default;

	~simple_vector() = default;

	simple_vector(std::initializer_list<T> init) noexcept
	{
		size_t init_size = init.size();
		data_ = array_ptr<T>(init_size);

		size_t i = 0;
		for (const T& value : init)
		{
			data_[i] = value;
			++i;
		}

		size_ = init_size;
		capacity_ = init_size;
	}

	simple_vector(const simple_vector& other)
	{
		size_t new_size = other.size_;

		array_ptr<T> new_data_(new_size);

		for (size_t i = 0; i < new_size; i++)
		{
			new_data_[i] = other.data_[i];
		}

		data_ = std::move(new_data_);
		size_ = new_size;
		capacity_ = new_size;
	}

	simple_vector(simple_vector&& other) noexcept { swap(other); }

	simple_vector& operator=(const simple_vector& other)
	{
		if (this != &other)
		{
			simple_vector temp(other);
			swap(temp);
		}
		return *this;
	}

	simple_vector(size_t size, const T& value = T{})
	{
		size_ = size;
		capacity_ = size;

		if (size > 0)
		{
			data_ = array_ptr<T>(size);
			for (size_t i = 0; i < size; i++)
			{
				data_[i] = value;
			}
		}
	}

	iterator begin() noexcept { return iterator(data_.get()); }

	iterator end() noexcept { return iterator(data_.get() + size_); }

	using const_iterator = iterator;

	const_iterator begin() const noexcept { return iterator(data_.get()); }

	const_iterator end() const noexcept
	{
		return iterator(data_.get() + size_);
	}

	typename iterator::reference operator[](size_t index) noexcept
	{
		return data_[index];
	}

	typename const_iterator::reference operator[](size_t index) const noexcept
	{
		return data_.get()[index];
	}

	typename iterator::reference at(size_t index)
	{
		if (index >= size_)
		{
			throw std::out_of_range("Index out of range");
		}
		return data_[index];
	}

	typename const_iterator::reference at(size_t index) const
	{
		if (index >= size_)
		{
			throw std::out_of_range("Index out of range");
		}
		return data_[index];
	}

	size_t size() const noexcept { return size_; }

	size_t capacity() const noexcept { return capacity_; }

	void swap(simple_vector& other) noexcept
	{
		size_t temp_size = size_;
		size_ = other.size_;
		other.size_ = temp_size;

		size_t temp_capacity = capacity_;
		capacity_ = other.capacity_;
		other.capacity_ = temp_capacity;

		array_ptr<T> temp_data = std::move(data_);
		data_ = std::move(other.data_);
		other.data_ = std::move(temp_data);
	}

	friend void swap(simple_vector& lhs, simple_vector& rhs) noexcept
	{
		lhs.swap(rhs);
	}

	void reserve(size_t new_cap)
	{
		if (new_cap <= capacity_)
		{
			return;
		}
		array_ptr<T> new_data(new_cap);

		for (size_t i = 0; i < size_; i++)
		{
			new_data[i] = std::move(data_[i]);
		}

		data_ = std::move(new_data);
		capacity_ = new_cap;
	}

	void resize(size_t new_size)
	{
		if (new_size > capacity_)
		{
			reserve(new_size);
		}
		if (new_size > size_)
		{
			for (size_t i = size_; i < new_size; i++)
			{
				data_[i] = T{};
			}
		}
		size_ = new_size;
	}

	iterator insert(const_iterator where, T&& value)
	{
		size_t index = where - begin();

		if (size_ == capacity_)
		{
			if (capacity_ == 0)
			{
				reserve(1);
			}
			else
			{
				reserve(capacity_ * 2);
			}
		}

		size_t i = size_;
		while (i > index)
		{
			data_[i] = data_[i - 1];
			--i;
		}

		data_[index] = std::move(value);
		++size_;

		return begin() + index;
	}

	iterator insert(const_iterator where, const T& value)
	{
		size_t index = where - begin();

		if (size_ == capacity_)
		{
			if (capacity_ == 0)
			{
				reserve(1);
			}
			else
			{
				reserve(capacity_ * 2);
			}
		}
		size_t i = size_;
		while (i > index)
		{
			data_[i] = data_[i - 1];
			--i;
		}

		data_[index] = value;
		++size_;

		return begin() + index;
	}

	void push_back(T&& value)
	{
		if (size_ == capacity_)
		{
			if (capacity_ == 0)
			{
				reserve(1);
			}
			else
			{
				reserve(capacity_ * 2);
			}
		}

		data_[size_] = std::move(value);

		++size_;
	}

	void clear() noexcept
	{
		for (size_t i = 0; i < size_; i++)
		{
			data_[i] = T{};
		}
		size_ = 0;
	}

	void push_back(const T& value)
	{
		if (size_ == capacity_)
		{
			T temp = std::move(value);
			size_t new_cap;
			if (capacity_ == 0)
			{
				new_cap = 1;
			}
			else
			{
				new_cap = capacity_ * 2;
			}
			reserve(new_cap);
			data_[size_] = std::move(temp);
		}
		else
		{
			data_[size_] = std::move(value);
		}
		++size_;
	}

	bool empty() const noexcept { return size_ == 0; }

	void pop_back()
	{
		if (size_ == 0)
		{
			return;
		}

		data_[size_ - 1] = T{};
		--size_;
	}

	friend bool operator==(const simple_vector& lhs, const simple_vector& rhs)
	{
		if (lhs.size() != rhs.size())
		{
			return false;
		}
		for (size_t i = 0; i < lhs.size(); i++)
		{
			if (lhs[i] != rhs[i])
			{
				return false;
			}
		}
		return true;
	}

	friend bool operator!=(const simple_vector& lhs, const simple_vector& rhs)
	{
		return !(lhs == rhs);
	}

	friend auto operator<=>(const simple_vector& lhs, const simple_vector& rhs)
	{
		for (size_t i = 0; i < std::min(lhs.size_, rhs.size_); i++)
		{
			if (lhs.data_[i] != rhs.data_[i])
			{
				return lhs.data_[i] <=> rhs.data_[i];
			}
		}
		return lhs.size_ <=> rhs.size_;
	}

	friend std::ostream& operator<<(std::ostream& os, const simple_vector& vec)
	{
		os << "[";
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (i > 0)
			{
				os << ",";
			}
			os << vec[i];
		}
		os << "]";
		return os;
	}
	iterator erase(iterator where)
	{
		size_t index = where - begin();
		for (size_t i = index; i < size_ - 1; i++)
		{
			data_[i] = data_[i + 1];
		}
		--size_;
		data_[size_] = T{};

		return begin() + index;
	}

   private:
	static bool alphabet_compare(const simple_vector<T>& lhs,
								 const simple_vector<T>& rhs)
	{
		size_t min_size = lhs.size();
		if (rhs.size() < min_size)
		{
			min_size = rhs.size();
		}

		for (size_t i = 0; i < min_size; i++)
		{
			if (lhs[i] < rhs[i])
			{
				return true;
			}
			if (lhs[i] > rhs[i])
			{
				return false;
			}
		}
		if (lhs.size() < rhs.size())
		{
			return true;
		}
		return false;
	}
	array_ptr<T> data_;
	size_t size_ = 0;
	size_t capacity_ = 0;
};
}  // namespace bmstu
