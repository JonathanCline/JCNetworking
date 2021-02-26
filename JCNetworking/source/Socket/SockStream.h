#pragma once

#include "SockPacket.h"

#include <list>



namespace JCN_NAMESPACE::bk
{

	template <typename T>
	class basic_sockbuff
	{
	public:
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using size_type = size_t;

		// Capacity of each packet in the buffer
		size_type pcapacity() const noexcept
		{
			return this->pcapacity_;
		};

		// Sets the capacity of packets added to the buffer. Only affects packets added after this call.
		void set_pcapacity(size_type _count)
		{
			this->pcapacity_ = _count;
		};

		size_type size() const noexcept
		{
			return this->packets_.size();
		};

		void clear() noexcept
		{
			this->packets_.clear();
		};

		bool empty() const noexcept
		{
			return this->packets_.empty();
		};

	private:
		reference front() { return this->packets_.front(); };
		const_reference front() const { return this->packets_.front(); };

		reference back() { return this->packets_.back(); };
		const_reference back() const { return this->packets_.back(); };

		// Adds a new packet to the end of the buffer
		reference push_back(size_type _capacity)
		{
			this->packets_.push_back(value_type{ _capacity });
			return this->back();
		};

		// Adds a new packet to the end of the buffer using pcapacity() as the _capacity value
		reference push_back()
		{
			return this->push_back(this->pcapacity());
		};
		
		void pop_front()
		{
			this->packets_.pop_front();
		};

		// Writes characters to the last packet in the buffer and returns the new '_pack' offset
		size_type write_back(const value_type& _pack, size_type _offset = 0, size_type _count = 0)
		{
			JCN_ASSERT(_offset <= _pack.size());
			JCN_ASSERT(_count <= _pack.size());
			JCN_ASSERT(_offset + _count <= _pack.size());

			if (this->empty() || this->back().size() == this->back().capacity())
			{
				this->push_back();
			};

			auto& _back = this->back();
			if (_count == 0)
			{
				_count = (_pack.size() - _offset);
			};

			const auto _bsize = _back.size();
			const auto _bcap = _back.capacity();
			const auto _bempty = _bcap - _bsize;

			const auto _rcount = std::min(_bempty, _count);

			auto* _bbegin = _back.data() + _back.size();

			const auto* _pbegin = _pack.data() + _offset;
			const auto* _pend = _pbegin + _rcount;

			std::copy(_pbegin, _pend, _bbegin);
			_back.resize(_back.size() + _rcount);

			return _offset + _rcount;
		};

	public:
		
		void write(const value_type& _pack, size_type _offset = 0, size_type _count = 0)
		{
			JCN_ASSERT(_offset <= _pack.size());
			JCN_ASSERT(_count <= _pack.size());
			JCN_ASSERT(_offset + _count <= _pack.size());

			if (_count == 0)
			{
				_count = (_pack.size() - _offset);
			};

			while (_offset < _count)
			{
				_offset = this->write_back(_pack, _offset, _count - _offset);
			};

		};
		size_type read_into(value_type& _pack, size_type _count = 0)
		{
			const auto _remaining = _pack.capacity() - _pack.size();
			if (_count == 0)
			{
				_count = _remaining;
			};
			JCN_ASSERT(_count <= _remaining);

			auto* _obegin = _pack.data() + _pack.size();
			size_type _gcount = 0;

			if (!this->empty())
			{
				auto& _front = this->front();
				const auto _rcount = std::min(_count, _front.size());
				
				auto _ibegin = _front.data();
				auto _iend = _ibegin + _front.size();

				std::copy_n(_ibegin, _rcount, _obegin);
				_gcount = _rcount;
				_pack.resize(_pack.size() + _gcount);

				std::rotate(_ibegin, _ibegin + _rcount, _iend);
				_front.resize(_front.size() - _rcount);
				if (_front.size() == 0)
				{
					this->pop_front();
				};
			};

			return _gcount;
		};




		basic_sockbuff() = default;
		basic_sockbuff(size_type _pcap) :
			pcapacity_{ _pcap }
		{};

	private:
		size_type pcapacity_ = 512;
		std::list<value_type> packets_{};

	};

	using sockbuff = basic_sockbuff<Buffer<char>>;

	template <typename T>
	class basic_sockbuff_ptr
	{
	public:
		using value_type = basic_sockbuff<T>;

		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using size_type = size_t;

	private:
		struct sdata
		{
			value_type buff;
			size_type refcount = 1;
			static sdata* addref_sdata(sdata* _other)
			{
				if (_other) [[likely]]
				{
					++_other->refcount;
				};
				return _other;
			};
			static sdata* deref_sdata(sdata*& _other)
			{
				if (_other) [[likely]]
				{
					--_other->refcount;
					if (_other->refcount == 0)
					{
						delete _other;
					};
					_other = nullptr;
				};
				return _other;
			};
	
		};

		auto& data() { return this->ptr_; };
		const auto& data() const { return this->ptr_; };

		auto& rcount() { return this->data()->refcount; };
		const auto& rcount() const { return this->data()->refcount; };
		
		auto& sbuff() { return this->data()->buff; };
		const auto& sbuff() const { return this->data()->buff; };
		
	public:

		bool good() const noexcept { return this->data() != nullptr; };
		explicit operator bool() const noexcept { return this->good(); };

		void release()
		{
			this->data() = nullptr;
		};
		void reset()
		{
			if (this->good())
			{
				auto& _data = this->data();
				_data = sdata::deref_sdata(_data);
				JCN_ASSERT(!this->good());
			};
		};

		auto& get()
		{
			JCN_ASSERT(this->good());
			return this->sbuff();
		};
		const auto& get() const
		{
			JCN_ASSERT(this->good());
			return this->sbuff();
		};

		auto& operator*()
		{
			return this->get();
		};
		const auto& operator*() const
		{
			return this->get();
		};

		pointer operator->()
		{
			return &this->get();
		};
		const_pointer operator->() const
		{
			return &this->get();
		};

		explicit basic_sockbuff_ptr() noexcept = default;
		explicit basic_sockbuff_ptr(size_type _pcapacity) noexcept :
			ptr_{ new sdata{ value_type{ _pcapacity }, 1 } }
		{};

		basic_sockbuff_ptr(const basic_sockbuff_ptr& other) : 
			ptr_{ sdata::addref_sdata(other.ptr_) }
		{};
		basic_sockbuff_ptr& operator=(const basic_sockbuff_ptr& other)
		{
			this->reset();
			JCN_ASSERT(!this->good());
			this->ptr_ = sdata::addref_sdata(other.ptr_);
			return *this;
		};

		basic_sockbuff_ptr(basic_sockbuff_ptr&& other) noexcept :
			ptr_{ std::exchange(other.ptr_, nullptr) }
		{};
		basic_sockbuff_ptr& operator=(basic_sockbuff_ptr&& other) noexcept
		{
			this->reset();
			this->ptr_ = std::exchange(other.ptr_, nullptr);
			return *this;
		};

		~basic_sockbuff_ptr()
		{
			this->reset();
		};

	private:
		sdata* ptr_ = nullptr;

	};

	using sockbuff_ptr = basic_sockbuff_ptr<Buffer<char>>;

};
