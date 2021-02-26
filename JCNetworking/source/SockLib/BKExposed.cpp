#include "BKExposed.h"

#include "SockLib.h"

namespace JCN_NAMESPACE::bk::impl
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

	SocketAdapter::socket_type SocketAdapter::unwrap_socket_ref(const socketref& _s)
	{
		return _s.get();
	};
	socketref SocketAdapter::wrap_socket_ref(const socket_type& _sock)
	{
		return socketref{ _sock };
	};
};

namespace JCN_NAMESPACE::bk::impl
{
	SocketAdapter::socket_type SocketUnwrapper::unwrap(const socket& _s)
	{
		return SocketAdapter::unwrap_socket(_s);
	};
	socket SocketUnwrapper::wrap(const SocketAdapter::socket_type& _s)
	{
		return SocketAdapter::wrap_socket(_s);
	};

	SocketAdapter::socket_type SocketUnwrapper::unwrap_ref(const socketref& _s)
	{
		return SocketAdapter::unwrap_socket_ref(_s);
	};
	socketref SocketUnwrapper::wrap_ref(const SocketAdapter::socket_type& _s)
	{
		return SocketAdapter::wrap_socket_ref(_s);
	};
};
