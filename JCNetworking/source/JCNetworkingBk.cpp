#include "JCNetworkingBk.h"

#include "SockLib/SockLib.h"
#include "SockLib/BKExposed.h"

#include "Socket/SockPacket.h"
#include "Socket/SockStream.h"



namespace JCN_NAMESPACE::bk
{

	namespace
	{
#ifdef JCN_WINSOCK
		auto set_blocking_mode_os(const slib::socket_t& _sock, bool _makeBlocking)
		{
			u_long _arg = ((_makeBlocking)? 1 : 0);
			return slib::ioctlsocket(_sock, FIONBIO, &_arg);
		};
#else
		auto set_blocking_mode_os(const slib::socket_t& _sock, bool _makeBlocking)
		{
			int _flags = slib::fcntl(_sock, F_GETFL, 0);
			if (!_makeBlocking)
			{
				_flags |= O_NONBLOCK;
			}
			else
			{
				_flags &= ~O_NONBLOCK;
			};
			return slib::fcntl(_sock, F_SETFL, _flags);
		};
#endif
	};


	result make_blocking(const slib::socket_t& _sock)
	{
		auto _result = set_blocking_mode_os(_sock, true);
		return (result)slib::check_result(_result);
	};
	result make_nonblocking(const slib::socket_t& _sock)
	{

		auto _result = set_blocking_mode_os(_sock, true);
		return (result)slib::check_result(_result);
	};


	result make_blocking(const socket& _sock)
	{
		return make_blocking(unwrap_socket(_sock));
	};
	result make_nonblocking(const socket& _sock)
	{
		return make_nonblocking(unwrap_socket(_sock));
	};



	/*

	std::optional<SOCKET> make_listener_socket(uint16_t _port, int _aiFamily, int _sockType, int _protoCol)
	{
		SOCKET _listenSock{};

		addrinfo* result = NULL;
		addrinfo* ptr = NULL;
		addrinfo hints{};

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = _aiFamily;
		hints.ai_socktype = _sockType;
		hints.ai_protocol = _protoCol;
		hints.ai_flags = AI_PASSIVE;

		auto iResult = getaddrinfo(NULL, std::to_string(_port).c_str(), &hints, &result);
		if (iResult != 0) [[unlikely]]
		{
			return std::nullopt;
		};

		_listenSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (_listenSock == INVALID_SOCKET) [[unlikely]]
		{
			//LOG_ERROR << ("Error on make socket call (socket()) : " + std::to_string(WSAGetLastError()));
			freeaddrinfo(result);
			return std::nullopt;
		};

		unsigned long ioArg = 1;
		iResult = make_non_blocking(_listenSock);
		if (iResult == SOCKET_ERROR) [[unlikely]]
		{
			//LOG_ERROR << ("Set socket to non-blocking failed with error : " + std::to_string(WSAGetLastError()));
			freeaddrinfo(result);
			return std::nullopt;
		};

		iResult = bind(_listenSock, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) [[unlikely]]
		{
			//LOG_ERROR << ("bind failed with error : " + std::to_string(WSAGetLastError()));
			freeaddrinfo(result);
			return std::nullopt;
		};

		iResult = listen(_listenSock, SOMAXCONN);
		if (iResult == SOCKET_ERROR) [[unlikely]]
		{
			//LOG_ERROR << ("listen failed with error : " + std::to_string(WSAGetLastError()));
			freeaddrinfo(result);
			return std::nullopt;
		};

		freeaddrinfo(result);
		return _listenSock;
	};

	*/


	namespace
	{
		socket listen(const char* _service, int _family, int _sockType, int _protocol)
		{
			slib::socket_t _listenSock{};

			addrinfo* result = NULL;
			addrinfo* ptr = NULL;
			addrinfo hints{};

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = _family;
			hints.ai_socktype = _sockType;
			hints.ai_protocol = _protocol;
			hints.ai_flags = AI_PASSIVE;

			auto iResult = slib::getaddrinfo(NULL, _service, &hints, &result);
			if (iResult != 0) [[unlikely]]
			{
				return socket{};
			};

			_listenSock = slib::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (_listenSock == slib::invalid_socket()) [[unlikely]]
			{
				slib::check_result(slib::bad_result(), "socket");
				slib::freeaddrinfo(result);
				return socket{};
			};

			unsigned long ioArg = 1;
			auto _result = make_nonblocking(_listenSock);
			if (!_result) [[unlikely]]
			{
				slib::freeaddrinfo(result);
				return socket{};
			};

			iResult = slib::bind(_listenSock, result->ai_addr, (int)result->ai_addrlen);
			if (!slib::check_result(iResult, "bind")) [[unlikely]]
			{
				//LOG_ERROR << ("bind failed with error : " + std::to_string(WSAGetLastError()));
				slib::freeaddrinfo(result);
				return socket{};
			};

			iResult = slib::listen(_listenSock, SOMAXCONN);
			if (!slib::check_result(iResult, "listen")) [[unlikely]]
			{
				//LOG_ERROR << ("listen failed with error : " + std::to_string(WSAGetLastError()));
				slib::freeaddrinfo(result);
				return socket{};
			};

			slib::freeaddrinfo(result);
			return wrap_socket(_listenSock);
		};
	};

	socket listen(const char* _pService, protocol::tcp_t)
	{
		return listen(_pService, AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	};
	socket listen(const char* _pService, protocol::udp_t)
	{
		return listen(_pService, AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	};

	socket listen(port_t _port, protocol::tcp_t)
	{
		std::string _pstr = std::to_string(_port);
		return listen(_pstr.c_str(), protocol::tcp);
	};
	socket listen(port_t _port, protocol::udp_t)
	{
		std::string _pstr = std::to_string(_port);
		return listen(_pstr.c_str(), protocol::tcp);
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

namespace JCN_NAMESPACE::bk
{

	packetbuff::pointer packetbuff::get() const noexcept
	{
		return this->ptr_;
	};
	packetbuff::pointer packetbuff::operator->() const noexcept
	{
		return this->get();
	};

	basic_sockbuff<packet>& packetbuff::sbuf() const noexcept
	{
		JCN_ASSERT(this->good());
		return this->get()->get();
	};

	bool packetbuff::good() const noexcept
	{
		return this->get() != nullptr;
	};
	packetbuff::operator bool() const noexcept
	{
		return this->good();
	};

	void packetbuff::release() noexcept
	{
		this->ptr_ = nullptr;
	};
	void packetbuff::reset()
	{
		if (this->good())
		{
			delete this->ptr_;
			this->release();
		};
	};
	packetbuff::pointer packetbuff::extract() noexcept
	{
		auto _out = this->get();
		this->release();
		return _out;
	};

	packetbuff::packetbuff(sockbuff_ptr* _ptr) noexcept :
		ptr_{ _ptr }
	{};
	packetbuff::packetbuff(size_t _packetCapacity) :
		packetbuff{ new value_type{ _packetCapacity } }
	{};
	packetbuff::packetbuff() :
		packetbuff{ 512 }
	{};
	packetbuff::packetbuff(std::nullptr_t _ptr) noexcept {};

	packetbuff::packetbuff(const packetbuff& other) :
		packetbuff{ ((other) ? new value_type{ *other.get() } : nullptr) }
	{};
	packetbuff& packetbuff::operator=(const packetbuff& other)
	{
		this->reset();
		if (other)
		{
			this->ptr_ = new value_type{ *other.get() };
		};
		return *this;
	};

	packetbuff::packetbuff(packetbuff&& other) noexcept :
		ptr_{ other.extract() }
	{};
	packetbuff& packetbuff::operator=(packetbuff&& other) noexcept
	{
		this->reset();
		this->ptr_ = other.extract();
		return *this;
	};

	packetbuff::~packetbuff()
	{
		this->reset();
	};

};

namespace JCN_NAMESPACE::bk
{
	bool Connection::good() const noexcept
	{
		return this->recvbuf() && this->sendbuf();
	};

	result Connection::connect(Connection& other)
	{
		JCN_ASSERT(!this->good());
		
		this->set_recvbuf(packetbuff{ 256 });
		other.set_sendbuf(this->recvbuf());

		this->set_sendbuf(packetbuff{ 256 });
		other.set_recvbuf(this->sendbuf());

		return result::rGood;
	};

	result Connection::send(const packet& _packet)
	{
		auto& _sb = this->sendbuf().sbuf();
		_sb.write(_packet);
		return result::rGood;
	};
	result Connection::send(char* _data, size_t _len)
	{
		packet _pk{ this->sendbuf().sbuf().pcapacity() };
		std::copy_n(_data, _len, _pk.data());
		_pk.resize(_len);
		return this->send(_pk);
	};
	
	result Connection::recv(packet& _readInto, size_t _max)
	{
		auto& _sb = this->recvbuf().sbuf();
		_sb.read_into(_readInto, _max);
		return result::rGood;
	};
	result Connection::recv(char* _readInto, size_t _max)
	{
		packet _pk{ this->recvbuf().sbuf().pcapacity() };
		auto _result = this->recv(_pk, _max);
		if (_result)
		{
			std::copy_n(_pk.data(), _pk.size(), _readInto);
		};
		return _result;
	};

	result Connection::close()
	{
		this->recvbuf_.reset();
		this->sendbuf_.reset();
		return result::rGood;
	};

	Connection::operator bool() const noexcept
	{
		return this->good();
	};

};

