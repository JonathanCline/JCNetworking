#include "SockSet.h"

namespace JCN_NAMESPACE::bk
{
	SockSet::size_type SockSet::max_size() noexcept
	{
		return FD_SETSIZE;
	};
	SockSet::size_type SockSet::size() const noexcept
	{
		return this->fd_.fd_count;
	};

	void SockSet::resize(size_type _count)
	{
		JCN_ASSERT(_count <= this->max_size());
		this->fd_.fd_count = _count;
	};

	bool SockSet::empty() const noexcept
	{
		return this->size() == 0;
	};

	SockSet::pointer SockSet::data() noexcept
	{
		return &this->fd_.fd_array[0];
	};
	SockSet::const_pointer SockSet::data() const noexcept
	{
		return &this->fd_.fd_array[0];
	};

	void SockSet::insert(const_reference _sock)
	{
		JCN_ASSERT(this->size() < this->max_size());
		this->fd_.fd_array[this->fd_.fd_count++] = _sock;
	};

	void SockSet::clear() noexcept
	{
		this->fd_.fd_count = 0;
	};

	void SockSet::assign(const value_type* _data, size_type _count)
	{
		JCN_ASSERT(_count <= this->max_size());
		this->resize(_count);
		for (size_type i = 0; i < _count; ++i)
		{
			this->at(i) = *_data;
		};
	};


	SockSet::reference SockSet::at(size_type _index)
	{
		JCN_ASSERT(_index < this->size());
		return this->fd_.fd_array[_index];
	};
	SockSet::const_reference SockSet::at(size_type _index) const
	{
		JCN_ASSERT(_index < this->size());
		return this->fd_.fd_array[_index];
	};

	SockSet::reference SockSet::operator[](size_type _index)
	{
		return this->at(_index);
	};
	SockSet::const_reference SockSet::operator[](size_type _index) const
	{
		return this->at(_index);
	};

	SockSet::SockSet(const value_type* _data, size_type _count) noexcept
	{
		JCN_ASSERT(_data != nullptr);
		JCN_ASSERT(_count <= this->max_size());
		this->assign(_data, _count);
	};
	SockSet::SockSet(std::initializer_list<value_type> _socks)
	{
		this->assign(_socks.begin(), _socks.end());
	};

	SockSet::SockSet(const SockSet& other) noexcept
	{
		this->assign(other.data(), other.size());
	};
	SockSet& SockSet::operator=(const SockSet& other) noexcept
	{
		this->assign(other.data(), other.size());
		return *this;
	};

	SockSet::SockSet(SockSet&& other) noexcept
	{
		this->assign(other.data(), other.size());
		other.clear();
	};
	SockSet& SockSet::operator=(SockSet&& other) noexcept
	{
		this->assign(other.data(), other.size());
		other.clear();
		return *this;
	};

};
