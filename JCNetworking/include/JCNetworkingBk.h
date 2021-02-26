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
	class socket;

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

		};
	};




	class socketref;

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

	socket connect(const char* _ipstr, const char* _pService, protocol::tcp_t);
	socket connect(const char* _ipstr, const char* _pService, protocol::udp_t);

	socket connect(const char* _ipstr, port_t _port, protocol::tcp_t);
	socket connect(const char* _ipstr, port_t _port, protocol::udp_t);


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








};


#endif