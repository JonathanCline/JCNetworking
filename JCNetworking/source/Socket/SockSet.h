#pragma once

#include "JCNetworkingSockLib.h"

#include <initializer_list>
#include <numeric>

namespace JCN_NAMESPACE::bk
{

	class SockSet
	{
	private:
		using fd_set = slib::fd_set;

	public:
		using value_type = slib::socket_t;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using size_type = size_t;

		static size_type max_size() noexcept;
		size_type size() const noexcept;

		void resize(size_type _count);

		bool empty() const noexcept;

		pointer data() noexcept;
		const_pointer data() const noexcept;

		void insert(const_reference _sock);

		void assign(const value_type* _data, size_type _count);

		template <typename IterT> 
		requires requires (slib::fd_set _fset, IterT _it)
		{
			_fset.fd_array[0] = *_it;
		}
		void assign(IterT _begin, const IterT _end)
		{
			const auto _count = std::distance(_begin, _end);
			JCN_ASSERT(_count <= this->max_size());

			auto* _fPtr = this->fd_.fd_array;
			for (_begin; _begin != _end; _begin = std::next(_begin))
			{
				(*_fPtr) = *_begin;
				++_fPtr;
			};

			this->fd_.fd_count = _count;
		};

		void clear() noexcept;

		reference at(size_type _index);
		const_reference at(size_type _index) const;

		reference operator[](size_type _index);
		const_reference operator[](size_type _index) const;

		SockSet() = default;

		explicit SockSet(const value_type* _data, size_type _count) noexcept;
		SockSet(std::initializer_list<value_type> _socks);

		SockSet(const SockSet& other) noexcept;
		SockSet& operator=(const SockSet& other) noexcept;

		SockSet(SockSet&& other) noexcept;
		SockSet& operator=(SockSet&& other) noexcept;

	private:
		fd_set fd_{};

	};


};
