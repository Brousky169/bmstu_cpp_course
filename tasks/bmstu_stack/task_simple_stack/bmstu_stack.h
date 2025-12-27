#pragma once

#include <exception>
#include <iostream>
#include <utility>

namespace bmstu
{
template <typename T>
class stack
{
   public:
	stack() : data_(nullptr), size_(0u) {}	// Конструктор

	bool empty() const noexcept { return size_ == 0u; }	 // Проверка на пустоту

	size_t size() const noexcept { return size_; }	// Возвращает текущий размер

	stack(const stack& other)
		: data_(nullptr), size_(0)	// Конструктор копирования
	{
		data_ = (T*)operator new(sizeof(T) * other.size_);
		size_ = other.size_;
		for (size_t i = 0; i < size_; ++i)
		{
			new (data_ + i) T(other.data_[i]);
		}
	}

	stack& operator=(const stack& other)  // Оператор копирующего присваивания
	{
		if (this != &other)
		{
			clear();
			operator delete(data_);
			data_ = (T*)operator new(sizeof(T) * other.size_);
			size_ = other.size_;
			for (size_t i = 0; i < size_; ++i)
			{
				new (data_ + i) T(other.data_[i]);
			}
		}
		return *this;
	}

	stack(stack&& other) noexcept
		: data_(nullptr), size_(0)	// Конструктор перемещения
	{
		data_ = other.data_;
		size_ = other.size_;
		other.data_ = nullptr;
		other.size_ = 0;
	}

	stack& operator=(
		stack&& other) noexcept	 // Оператор перемещающего присваивания
	{
		if (this != &other)
		{
			clear();
			operator delete(data_);
			data_ = other.data_;
			size_ = other.size_;
			other.data_ = nullptr;
			other.size_ = 0;
		}
		return *this;
	}

	T* data() const { return data_; }

	~stack()  // Деструктор
	{
		if (data_ != nullptr)
		{
			operator delete(data_);
		}
	}

	template <typename... Args>
	void emplace(Args&&... args)  // Создание новoго эл-та
	{
		T* new_data = (T*)(operator new(sizeof(T) * (size_ + 1)));

		for (size_t i = 0; i < size_; ++i)
		{
			new (&new_data[i]) T(std::move(data_[i]));
			data_[i].~T();
		}
		new (&new_data[size_]) T(std::forward<Args>(args)...);
		operator delete(data_);

		data_ = new_data;
		++size_;
	}

	void push(T&& value)  // Перемещение
	{
		T* new_data = (T*)(operator new(sizeof(T) * (size_ + 1)));

		for (size_t i = 0; i < size_; ++i)
		{
			new (&new_data[i]) T(std::move(data_[i]));
			data_[i].~T();
		}

		new (&new_data[size_]) T(std::forward<T>(value));
		operator delete(data_);

		data_ = new_data;
		++size_;
	}

	void clear() noexcept  // Очищение стека
	{
		for (size_t i = 0; i < size_; ++i)
		{
			data_[i].~T();
		}
		operator delete(data_);
		data_ = nullptr;
		size_ = 0;
	}

	void push(const T& value)  // Копирование
	{
		T* new_data = (T*)(operator new(sizeof(T) * (size_ + 1)));

		for (size_t i = 0; i < size_; ++i)
		{
			new (&new_data[i]) T(std::move(data_[i]));
			data_[i].~T();
		}
		new (&new_data[size_]) T(value);
		operator delete(data_);

		data_ = new_data;
		++size_;
	}

	void pop()	// Удаление верхнего эл-та
	{
		if (empty())
		{
			throw std::underflow_error("Stack is empty");
		}
		--size_;
		data_[size_].~T();
	}

	T& top()  // Возвращает ссылку на верхний эл-т (с возможным изменением)
	{
		if (empty())
		{
			throw std::underflow_error("Stack is empty");
		}
		return data_[size_ - 1];
	}

	const T& top() const  // Возвращает ссылку на верхний эл-т (только чтение)
	{
		if (empty())
		{
			throw std::underflow_error("Stack is empty");
		}
		return data_[size_ - 1];
	}

   private:
	T* data_;
	size_t size_;
};
}  // namespace bmstu
