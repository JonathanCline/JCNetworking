#pragma once
#ifndef JC_NETWORKING_BK_H
#define JC_NETWORKING_BK_H

#include "JCNetworkingConfig.h"

#include <array>
#include <string>
#include <chrono>

namespace JCN_NAMESPACE
{
	enum result : bool
	{
		rError = false,
		rGood = true
	};
};

namespace JCN_NAMESPACE::bk
{
	using JCN_NAMESPACE::result;

	class socket;
	class socketref;

	namespace impl
	{
		struct SocketUnwrapper;
		struct SocketAdapter
		{
		private:
			friend socket;

#ifdef JCN_WINSOCK
			using socket_type = size_t;
#else
			using socket_type = int;
#endif
			static socket_type null_value() noexcept;

			static void close_socket(socket_type& _sock);

			friend SocketUnwrapper;

		public:
			static socket_type unwrap_socket(const socket& _s);
			static socket wrap_socket(const socket_type& _sock);

			static socket_type unwrap_socket_ref(const socketref& _s);
			static socketref wrap_socket_ref(const socket_type& _sock);

		};
	};

	class socket
	{
	private:
		using SocketAdapter = impl::SocketAdapter;

		friend SocketAdapter;
		friend socketref;

		using value_type = SocketAdapter::socket_type;

		static value_type get_invalid_value() noexcept { return SocketAdapter::null_value(); };

		constexpr value_type& get() noexcept { return this->sock_; };
		constexpr const value_type& get() const noexcept { return this->sock_; };

	public:
		bool good() const noexcept
		{
			return this->get() != SocketAdapter::null_value();
		};
		explicit operator bool() const noexcept
		{
			return this->good();
		};

	protected:
		void release() noexcept
		{
			this->get() = 0;
		};
		void reset()
		{
			if (this->good())
			{
				SocketAdapter::close_socket(this->get());
				this->release();
			};
		};
		value_type extract() noexcept
		{
			auto _out = this->get();
			this->release();
			return _out;
		};

	public:
		constexpr bool operator==(const socket& other) const noexcept
		{
			return (this->get() == other.get());
		};
		constexpr bool operator!=(const socket& other) const noexcept
		{
			return (this->get() != other.get());
		};

		socket(const socket& other) = delete;
		socket& operator=(const socket& other) = delete;

		socket(socket&& other) noexcept :
			sock_{ other.extract() }
		{};
		socket& operator=(socket&& other) noexcept
		{
			this->reset();
			this->sock_ = other.extract();
			return *this;
		};
		
		socket() noexcept : 
			sock_{ SocketAdapter::null_value() }
		{};
		~socket()
		{
			this->reset();
		};

	private:
		constexpr explicit socket(value_type _sock) noexcept :
			sock_{ _sock }
		{}; 
		socket& operator=(value_type _sock) noexcept
		{
			this->reset();
			this->sock_ = _sock;
			return *this;
		};

		value_type sock_{};

	};

	class socketref
	{
	private:
		friend impl::SocketAdapter;
		using value_type = socket::value_type;

		constexpr static value_type& get(socket& _s) noexcept { return _s.get(); };
		constexpr static const value_type& get(const socket& _s) noexcept { return _s.get(); };

		constexpr value_type& get() noexcept { return this->sock_; };
		constexpr const value_type& get() const noexcept { return this->sock_; };

	public:
		bool good() const noexcept
		{
			return this->get() != socket::get_invalid_value();
		};
		explicit operator bool() const noexcept
		{
			return this->good();
		};

		friend inline bool operator==(const socketref& lhs, const socketref& rhs) noexcept
		{
			return (lhs.get() == rhs.get());
		};
		friend inline bool operator!=(const socketref& lhs, const socketref& rhs) noexcept
		{
			return (lhs.get() == rhs.get());
		};

		friend inline bool operator==(const socket& lhs, const socketref& rhs) noexcept
		{
			return (get(lhs) == rhs.get());
		};
		friend inline bool operator!=(const socket& lhs, const socketref& rhs) noexcept
		{
			return (get(lhs) != rhs.get());
		};

		friend inline bool operator==(const socketref& lhs, const socket& rhs) noexcept
		{
			return (get(rhs) == lhs.get());
		};
		friend inline bool operator!=(const socketref& lhs, const socket& rhs) noexcept
		{
			return (get(rhs) != lhs.get());
		};

		constexpr socketref() noexcept = default;

		constexpr socketref(const socket& _sock) :
			sock_{ _sock.get() }
		{};
		socketref& operator=(const socket& _sock)
		{
			this->sock_ = get(_sock);
			return  *this;
		};

	private:
		constexpr explicit socketref(const value_type& _sock) :
			sock_{ _sock }
		{}; 
		socketref& operator=(const value_type& _sock)
		{
			this->sock_ = _sock;
			return  *this;
		};


		value_type sock_{};

	};


	result make_blocking(const socket& _sock);
	result make_nonblocking(const socket& _sock);





	struct protocol
	{
		struct tcp_t {};
		constexpr static tcp_t tcp{};
		
		struct udp_t {};
		constexpr static udp_t udp{};
	};


	using port_t = uint16_t;

	
	


	socket listen(const char* _pService, protocol::tcp_t);
	socket listen(const char* _pService, protocol::udp_t);

	socket listen(port_t _port, protocol::tcp_t);
	socket listen(port_t _port, protocol::udp_t);


	
	
	socket connect(const char* _ipstr, const char* _pService, protocol::tcp_t);
	socket connect(const char* _ipstr, const char* _pService, protocol::udp_t);

	socket connect(const char* _ipstr, port_t _port, protocol::tcp_t);
	socket connect(const char* _ipstr, port_t _port, protocol::udp_t);
	
	




	template <typename T, typename AllocatorT = std::allocator<T>> class Buffer;
	using packet = Buffer<char>;

	template <typename T> class basic_sockbuff;
	

	template <typename T> class basic_sockbuff_ptr;
	using sockbuff_ptr = basic_sockbuff_ptr<packet>;

	class packetbuff
	{
	public:
		using value_type = sockbuff_ptr;
		using pointer = value_type*;

		basic_sockbuff<packet>& sbuf() const noexcept;

		pointer get() const noexcept;
		pointer operator->() const noexcept;

		bool good() const noexcept;
		explicit operator bool() const noexcept;

		void release() noexcept;
		void reset();
		pointer extract() noexcept;

		packetbuff();
		packetbuff(size_t _packetCapacity);
		explicit packetbuff(sockbuff_ptr* _ptr) noexcept;
		explicit packetbuff(std::nullptr_t _ptr) noexcept;

		packetbuff(const packetbuff& other);
		packetbuff& operator=(const packetbuff& other);
		
		packetbuff(packetbuff&& other) noexcept;
		packetbuff& operator=(packetbuff&& other) noexcept;
		
		~packetbuff();

	private:
		sockbuff_ptr* ptr_ = nullptr;
	};


	/*
	struct ios_packetstream_base
	{
	public:
		packetbuff rdbuf() const
		{
			return this->buff_;
		};
		void set_rdbuf(packetbuff _buf)
		{
			this->buff_ = _buf;
		};

		ios_packetstream_base() = default;
		ios_packetstream_base(packetbuff _buff) :
			buff_{ std::move(_buff) }
		{};

	private:
		packetbuff buff_;

	};

	struct ipacketstream : public ios_packetstream_base
	{
	public:



	private:
	
	};

	struct opacketstream : public ios_packetstream_base
	{
	public:


	private:

	};

	struct iopacketstream : public ios_packetstream_base
	{
	public:


	private:

	};
	*/

	/*
	struct send_flags
	{
		using value_type = int;
		enum : value_type
		{
			msgOOB,
			msgDontRoute,
			msgDontWait,
			msgNoSignal
		};
	};
	*/

	size_t send(const socket& _socket, const char* _buffer, size_t _bufferLen);





	class Connection
	{
	public:

		bool good() const noexcept;

		result connect(Connection& other);

		result close();

		result send(const packet& _packet);
		result send(char* _data, size_t _len);

		result recv(packet& _readInto, size_t _max = 0);
		result recv(char* _readInto, size_t _max);


		explicit operator bool() const noexcept;

		const packetbuff& sendbuf() const noexcept { return this->sendbuf_; };
		void set_sendbuf(const packetbuff& _sbuff) { this->sendbuf_ = _sbuff; };

		const packetbuff& recvbuf() const noexcept { return this->recvbuf_; };
		void set_recvbuf(const packetbuff& _sbuff) { this->recvbuf_ = _sbuff; };


		Connection() = default;

	private:
		packetbuff recvbuf_{ nullptr };
		packetbuff sendbuf_{ nullptr };

	};





	/*
	
	struct ConnectionTerminal
	{
		packetbuff recv;
		packetbuff send;
	};

	std::pair<ConnectionTerminal, ConnectionTerminal> make_connection()
	{
		ConnectionTerminal _lhs {};
		ConnectionTerminal _rhs
		{
			_lhs.send,
			_lhs.recv
		};
		return { std::move(_lhs), std::move(_rhs) };
	};

	void foo()
	{
		auto _conns = make_connection();

		auto a = _conns.first;
		auto b = _conns.second;

	};

	*/

	



};


#endif