#pragma once

#include "JCNetworkingConfig.h"

#include <cstdint>
#include <memory>
#include <type_traits>

namespace JCN_NAMESPACE::bk
{
	/**
	 * @brief Fixed sized dynamically allocated buffer
	 * @tparam T Value type
	 * @tparam AllocatorT Allocator type
	*/
	template <typename T, typename AllocatorT = std::allocator<T>>
	class Buffer
	{
	public:
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using allocator_type = AllocatorT;

		using size_type = size_t;

		size_type capacity() const noexcept
		{
			return this->capacity_;
		};
		size_type size() const noexcept
		{
			return this->size_;
		};
		void resize(size_type _count)
		{
			JCN_ASSERT(_count <= this->capacity());
			this->size_ = _count;
		};

		pointer data() noexcept
		{
			return this->data_;
		};
		const_pointer data() const noexcept
		{
			return this->data_;
		};

	private:
		allocator_type& get_allocator() const noexcept
		{
			return this->alloc_;
		};
		pointer allocate(size_type _count, value_type _fillVal = value_type{})
		{
			auto& _alloc = this->get_allocator();
			auto _out = _alloc.allocate(_count);
			JCN_ASSERT(_out != nullptr);
			std::fill_n(_out, _count, _fillVal);
			return _out;
		};

		void deallocate(pointer _data, size_type _count)
		{
			auto& _alloc = this->get_allocator();
			_alloc.deallocate(_data, _count);
		};

		void cleanup()
		{
			this->deallocate(this->data(), this->capacity());
			this->data_ = nullptr;
			this->size_ = 0;
		};
		void construct(size_type _size = 0)
		{
			JCN_ASSERT(_size <= this->capacity());
			this->cleanup();
			this->data_ = this->allocate(this->capacity());
			this->size_ = _size;
			if constexpr (std::is_default_constructible_v<value_type> && std::is_copy_assignable_v<value_type>)
			{
				std::fill_n(this->data(), this->capacity(), value_type{});
			};
		};

	public:
		reference at(size_type _idx)
		{
			JCN_ASSERT(_idx < this->size());
			return *(this->data() + _idx);
		};
		const_reference at(size_type _idx) const
		{
			JCN_ASSERT(_idx < this->size());
			return *(this->data() + _idx);
		};

		reference operator[](size_type _idx)
		{
			return this->at(_idx);
		};
		const_reference operator[](size_type _idx) const
		{
			return this->at(_idx);
		};

		void clear() noexcept
		{
			if constexpr (std::is_default_constructible_v<value_type>)
			{
				std::fill_n(this->data(), this->capacity(), value_type{});
			};
			this->size_ = 0;
		};

		Buffer(const size_type _capacity, allocator_type _alloc = allocator_type{}) :
			alloc_{ std::move(_alloc) }, capacity_{ _capacity }, size_{ 0 }
		{
			this->construct(this->size());
		};
		
		// Transfers ownership of memory to the buffer
		constexpr explicit Buffer(const size_type _len, pointer _data) noexcept :
			capacity_{ _len }, size_{ _len }, data_{ _data }
		{};

		Buffer(const Buffer& other) :
			alloc_{ other.get_allocator() }, capacity_{ other.capacity() },
			size_{ other.size() }, data_{ nullptr }
		{
			this->construct(this->size());
		};

		Buffer(Buffer&& other) noexcept :
			alloc_{ std::move(other.alloc_) }, capacity_{ other.capacity_ },
			size_{ std::exchange(other.size_, 0) }, data_{ std::exchange(other.data_, nullptr) }
		{};

		~Buffer()
		{
			this->cleanup();
		};

	private:
		mutable allocator_type alloc_{};
		const size_type capacity_ = 0;
		size_type size_ = 0;
		pointer data_ = nullptr;
	};

}