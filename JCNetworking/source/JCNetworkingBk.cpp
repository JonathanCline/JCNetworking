#include "JCNetworkingBk.h"

#include "SockLib/SockLib.h"


namespace JCN_NAMESPACE::bk
{
	namespace impl
	{
		SocketAdapter::socket_type SocketAdapter::null_value() noexcept
		{
			return slib::invalid_socket();
		};
		void SocketAdapter::close_socket(socket_type& _sock)
		{
			auto _result = slib::closesocket(_sock);
			slib::check_result(_result);
			_sock = SocketAdapter::null_value();
		};

		SocketAdapter::socket_type SocketAdapter::unwrap_socket(const socket& _s)
		{
			return _s.get();
		};
		socket SocketAdapter::wrap_socket(const socket_type& _sock)
		{
			return socket{ _sock };
		};

		struct SocketUnwrapper
		{
			static SocketAdapter::socket_type unwrap(const socket& _s)
			{
				return SocketAdapter::unwrap_socket(_s);
			};
			static socket wrap(const SocketAdapter::socket_type& _s)
			{
				return SocketAdapter::wrap_socket(_s);
			};
		};

	};

	auto unwrap_socket(const socket& _s)
	{
		return impl::SocketUnwrapper::unwrap(_s);
	};
	auto wrap_socket(const slib::socket_t& _s)
	{
		return impl::SocketUnwrapper::wrap(_s);
	};





	namespace
	{
#ifdef JCN_WINSOCK
		auto set_blocking_mode_os(const socket& _sock, bool _makeBlocking)
		{
			u_long _arg = ((_makeBlocking)? 1 : 0);
			return slib::ioctlsocket(unwrap_socket(_sock), FIONBIO, &_arg);
		};
#else
		auto set_blocking_mode_os(const socket& _sock, bool _makeBlocking)
		{
			auto _usock = unrap_socket(_sock);
			int _flags = slib::fcntl(_usock, F_GETFL, 0);
			if (!_makeBlocking)
			{
				_flags |= O_NONBLOCK;
			}
			else
			{
				_flags &= ~O_NONBLOCK;
			};
			return slib::fcntl(_usock, F_SETFL, _flags);
		};
#endif
	};
	
	result make_blocking(const socket& _sock)
	{
		auto _result = set_blocking_mode_os(_sock, true);
		return (result)slib::check_result(_result);
	};
	result make_nonblocking(const socket& _sock)
	{
		auto _result = set_blocking_mode_os(_sock, false);
		return (result)slib::check_result(_result);
	};



	namespace
	{
		socket connect(const char* _ipstr, const char* _pService, int _sockType, int _protocol)
		{
			constexpr auto flocation = "bk::connect()";

			slib::socket_t _clientSock = slib::invalid_socket();

			const char* _ipCString = _ipstr;
			const char* _portCString = _pService;

			addrinfo* result = nullptr;
			addrinfo* ptr = nullptr;
			addrinfo hints{};

			char recvbuff[512]{ 0 };
			std::fill_n(recvbuff, 512, 0);

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = _sockType;
			hints.ai_protocol = _protocol;

			// Resolve the server address and port
			auto _result = slib::getaddrinfo(_ipCString, _portCString, &hints, &result);
			if (!slib::check_result(_result, flocation))
			{
				slib::freeaddrinfo(result);
				return socket{};
			};

			// Attempt to connect to an address until one succeeds
			for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
			{
				// Create a SOCKET for connecting to server
				_clientSock = slib::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
				if (_clientSock == slib::invalid_socket())
				{
					slib::freeaddrinfo(result);
					slib::check_result(slib::bad_result(), flocation);
					return socket{};
				};

				// try connect
				_result = slib::connect(_clientSock, ptr->ai_addr, (int)ptr->ai_addrlen);
				if (!slib::check_result(_result, flocation))
				{
					slib::freeaddrinfo(result);
					_result = slib::closesocket(_clientSock);
					if (!slib::check_result(_result, flocation))
					{
						return socket{};
					};
				}
				else
				{
					break;
				};
			};

			slib::freeaddrinfo(result);
			return wrap_socket(_clientSock);

		};

	};

	socket connect(const char* _ipstr, const char* _pService, protocol::tcp_t)
	{
		return connect(_ipstr, _pService, SOCK_STREAM, IPPROTO_TCP);
	};
	socket connect(const char* _ipstr, const char* _pService, protocol::udp_t)
	{
		return connect(_ipstr, _pService, SOCK_DGRAM, IPPROTO_UDP);
	};

	socket connect(const char* _ipstr, port_t _port, protocol::tcp_t)
	{
		std::string _pstr = std::to_string(_port);
		return connect(_ipstr, _pstr.c_str(), protocol::tcp);
	};
	socket connect(const char* _ipstr, port_t _port, protocol::udp_t)
	{
		std::string _pstr = std::to_string(_port);
		return connect(_ipstr, _pstr.c_str(), protocol::udp);
	};

	size_t send(const socket& _socket, const char* _buffer, size_t _bufferLen)
	{
		JCN_ASSERT(_bufferLen <= std::numeric_limits<int>::max());
		auto _result = slib::send(unwrap_socket(_socket), _buffer, (int)_bufferLen, 0);
		if (!slib::check_result(_result, "bk::send()"))
		{
			return std::numeric_limits<size_t>::max();
		};
		return (size_t)std::min(_result, 0);
	};







}
